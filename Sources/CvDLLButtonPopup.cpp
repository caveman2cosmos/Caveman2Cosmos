// buttonPopup.cpp

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvCity.h"
#include "CvDLLButtonPopup.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvInitCore.h"
#include "CvMap.h"
#include "CvMessageControl.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPopupInfo.h"
#include "CvPopupReturn.h"
#include "CvPython.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvUnitSelectionCriteria.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"

// Public Functions...

#define PASSWORD_DEFAULT (L"*****")

CvDLLButtonPopup* CvDLLButtonPopup::m_pInst = NULL;

CvDLLButtonPopup& CvDLLButtonPopup::getInstance()
{
	if (m_pInst == NULL)
	{
		m_pInst = new CvDLLButtonPopup;
	}
	return *m_pInst;
}

void CvDLLButtonPopup::freeInstance()
{
	delete m_pInst;
	m_pInst = NULL;
}

CvDLLButtonPopup::CvDLLButtonPopup()
{
}


CvDLLButtonPopup::~CvDLLButtonPopup()
{
}

void CvDLLButtonPopup::OnAltExecute(CvPopup& popup, const PopupReturn& popupReturn, CvPopupInfo &info)
{
	CvPopupInfo* pInfo = new CvPopupInfo;
	if (pInfo)
	{
		*pInfo = info;
		gDLL->getInterfaceIFace()->addPopup(pInfo);
		gDLL->getInterfaceIFace()->popupSetAsCancelled(&popup);
	}
}


void CvDLLButtonPopup::OnOkClicked(CvPopup* pPopup, PopupReturn *pPopupReturn, CvPopupInfo &info)
{
	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_TEXT:
		break;

	case BUTTONPOPUP_CONFIRM_MENU:
		if ( pPopupReturn->getButtonClicked() == 0 )
		{
			switch (info.getData1())
			{
			case 0:
				Cy::call(PYCivModule, "gameExitSave");
				gDLL->SetDone(true);
				break;
			case 1:
				Cy::call(PYCivModule, "gameExitSave");
				gDLL->getInterfaceIFace()->exitingToMainMenu();
				break;
			case 2:
				GC.getGame().doControl(CONTROL_RETIRE);
				break;
			case 3:
				GC.getGame().regenerateMap();
				break;
			case 4:
				GC.getGame().doControl(CONTROL_WORLD_BUILDER);
				break;
			}
		}
		break;

	case BUTTONPOPUP_MAIN_MENU:
		if (pPopupReturn->getButtonClicked() == 0)
		{	// exit to desktop
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(0);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == 1)
		{	// exit to main menu
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(1);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == 2)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(2);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == 3)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(3);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == 4)
		{	// load game
			GC.getGame().doControl(CONTROL_LOAD_GAME);
		}
		else if (pPopupReturn->getButtonClicked() == 5)
		{	// save game
			GC.getGame().doControl(CONTROL_SAVE_NORMAL);
		}
		else if (pPopupReturn->getButtonClicked() == 6)
		{	// options
			Cy::call("CvScreensInterface", "showOptionsScreen");
		}
		else if (pPopupReturn->getButtonClicked() == 7)
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CONFIRM_MENU);
			if (NULL != pInfo)
			{
				pInfo->setData1(4);
				gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), true);
			}
		}
		else if (pPopupReturn->getButtonClicked() == 8)
		{	// Game details
			GC.getGame().doControl(CONTROL_ADMIN_DETAILS);
		}
		else if (pPopupReturn->getButtonClicked() == 9)
		{	// player details
			GC.getGame().doControl(CONTROL_DETAILS);
		}
		// 10 - cancel
		break;

	case BUTTONPOPUP_DECLAREWARMOVE:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			CvMessageControl::getInstance().sendChangeWar((TeamTypes)info.getData1(), true);
		}
		if (((pPopupReturn->getButtonClicked() == 0) || info.getOption2()) && info.getFlags() == 0)
		{
			GC.getGame().selectionListGameNetMessageInternal(GAMEMESSAGE_PUSH_MISSION, MISSION_MOVE_TO, info.getData2(), info.getData3(), info.getFlags(), false, info.getOption1(), false);
		}
		break;

	case BUTTONPOPUP_CONFIRMCOMMAND:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			int iAction = info.getData1();
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, GC.getActionInfo(iAction).getCommandType(), GC.getActionInfo(iAction).getCommandData(), -1, 0, info.getOption1());
		}
		break;

	case BUTTONPOPUP_LOADUNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			const CvSelectionGroup* pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();
			if (pSelectionGroup != NULL)
			{
				int iCount = pPopupReturn->getButtonClicked();

				const CvPlot* pPlot = pSelectionGroup->plot();
				foreach_ (const CvUnit* unit, pPlot->units())
				{
					if (pSelectionGroup->canDoCommand(COMMAND_LOAD_UNIT, unit->getOwner(), unit->getID()))
					{
						iCount--;
						if (iCount == 0)
						{
							GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, COMMAND_LOAD_UNIT, unit->getOwner(), unit->getID());
							break;
						}
					}
				}
			}
		}
		break;

	case BUTTONPOPUP_LEADUNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			const CvSelectionGroup* pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();

			if (pSelectionGroup != NULL)
			{
				int iCount = pPopupReturn->getButtonClicked();

				const CvPlot* pPlot = pSelectionGroup->plot();
				foreach_ (const CvUnit* unit, pPlot->units() | filtered(bind(&CvUnit::canPromote, _1, (PromotionTypes)info.getData1(), info.getData2())))
				{
					if (--iCount == 0)
					{
						GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_LEAD, unit->getID());
						break;
					}
				}
			}
		}
		break;

	case BUTTONPOPUP_DOESPIONAGE:
		if (pPopupReturn->getButtonClicked() != NO_ESPIONAGEMISSION)
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_ESPIONAGE, (EspionageMissionTypes) pPopupReturn->getButtonClicked());
		}
		break;

	case BUTTONPOPUP_DOESPIONAGE_TARGET:
		if (pPopupReturn->getButtonClicked() != -1)
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_ESPIONAGE, (EspionageMissionTypes)info.getData1(), pPopupReturn->getButtonClicked());
		}
		break;

	case BUTTONPOPUP_CHOOSETECH:
		if (pPopupReturn->getButtonClicked() == GC.getNumTechInfos())
		{
			Cy::call("CvScreensInterface", "showTechChooser");
			GET_PLAYER(GC.getGame().getActivePlayer()).chooseTech(0, "", true);
		}
		break;

	case BUTTONPOPUP_RAZECITY:
		if (pPopupReturn->getButtonClicked() == 1)
		{
			CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_RAZE, -1, -1, false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() == 2)
		{
			CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_KEEP, GC.getGame().getActivePlayer(), -1, false, false, false, false);
			}

			CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_GIFT, info.getData2(), -1, false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() == 3)
		{
			CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				gDLL->getInterfaceIFace()->selectCity(pCity, false);
			}

			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_RAZECITY, info.getData1(), info.getData2(), info.getData3());
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), false, true);
		}
		else if (pPopupReturn->getButtonClicked() == 0)
		{
			CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				pCity->chooseProduction();
				CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_KEEP, GC.getGame().getActivePlayer(), -1, false, false, false, false);
			}
		}
		break;

	case BUTTONPOPUP_DISBANDCITY:
		if (pPopupReturn->getButtonClicked() == 1)
		{
			CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_DISBAND, -1, -1, false, false, false, false);
		}
		else if (pPopupReturn->getButtonClicked() == 0)
		{
			CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				pCity->chooseProduction();
				CvMessageControl::getInstance().sendDoTask(info.getData1(), TASK_KEEP, GC.getGame().getActivePlayer(), -1, false, false, false, false);
			}
		}
// BUG - Examine Culture Flip - start
		else if (pPopupReturn->getButtonClicked() == 2)
		{
			CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());
			if (NULL != pCity)
			{
				gDLL->getInterfaceIFace()->selectCity(pCity, false);
			}

			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DISBANDCITY, info.getData1());
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), false, true);
		}
// BUG - Examine Culture Flip - end
		break;

	case BUTTONPOPUP_CHOOSEPRODUCTION: {

		int iExamineCityID = 0;
		iExamineCityID = std::max(iExamineCityID, GC.getNumUnitInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumProjectInfos());
		iExamineCityID = std::max(iExamineCityID, GC.getNumProcessInfos());

		if (pPopupReturn->getButtonClicked() == iExamineCityID)
		{
			CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());
			if (pCity != NULL)
			{
				gDLL->getInterfaceIFace()->selectCity(pCity, true);
			}
		}
		break;
	}

	case BUTTONPOPUP_CHANGECIVIC:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			std::vector<CivicTypes> aeNewCivics(GC.getNumCivicOptionInfos());
			for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			{
				aeNewCivics[iI] = GET_PLAYER(GC.getGame().getActivePlayer()).getCivics((CivicOptionTypes)iI);
			}
			aeNewCivics[info.getData1()] = ((CivicTypes)(info.getData2()));

			CvMessageControl::getInstance().sendUpdateCivics(aeNewCivics);
		}
		else if (pPopupReturn->getButtonClicked() == 2)
		{
			Cy::call(PYScreensModule, "showCivicsScreen");
		}
		break;

	case BUTTONPOPUP_CHANGERELIGION:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			CvMessageControl::getInstance().sendConvert((ReligionTypes)(info.getData1()));
		}
		break;

	case BUTTONPOPUP_CHOOSEELECTION:
		{
			VoteSelectionData* pData = GC.getGame().getVoteSelection(info.getData1());
			if (NULL != pData && pPopupReturn->getButtonClicked() < (int)pData->aVoteOptions.size())
			{
				CvMessageControl::getInstance().sendChooseElection((VoteTypes)(pPopupReturn->getButtonClicked()), info.getData1());
			}
		}
		break;

	case BUTTONPOPUP_DIPLOVOTE:
		CvMessageControl::getInstance().sendDiploVote(info.getData1(), (PlayerVoteTypes)pPopupReturn->getButtonClicked());
		break;

	case BUTTONPOPUP_ALARM:
		break;

	case BUTTONPOPUP_DEAL_CANCELED:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			gDLL->sendKillDeal(info.getData1(), info.getOption1());
		}
		break;

	case BUTTONPOPUP_PYTHON:
		if (!info.getOnClickedPythonCallback().IsEmpty())
		{
			FAssertMsg(!GC.getGame().isNetworkMultiPlayer(), "Danger: Out of Sync");
			Cy::call((info.getPythonModule().IsEmpty() ? PYScreensModule : info.getPythonModule()), info.getOnClickedPythonCallback(), Cy::Args()
				<< pPopupReturn->getButtonClicked()
				<< info.getData1()
				<< info.getData2()
				<< info.getData3()
				<< info.getFlags()
				<< info.getText()
				<< info.getOption1()
				<< info.getOption2()
			);
			break;
		}
		break;

	case BUTTONPOPUP_DETAILS:
		{
			// Civ details
			PlayerTypes eID = GC.getInitCore().getActivePlayer();

			CvWString szLeaderName = GC.getInitCore().getLeaderName(eID);
			CvWString szCivDescription = GC.getInitCore().getCivDescription(eID);
			CvWString szCivShortDesc = GC.getInitCore().getCivShortDesc(eID);
			CvWString szCivAdjective = GC.getInitCore().getCivAdjective(eID);
			CvWString szCivPassword = PASSWORD_DEFAULT;
			CvString szEmail = GC.getInitCore().getEmail(eID);
			CvString szSmtpHost = GC.getInitCore().getSmtpHost(eID);

			if (pPopupReturn->getEditBoxString(0) && *(pPopupReturn->getEditBoxString(0)))
			{
				szLeaderName = pPopupReturn->getEditBoxString(0);
			}
			if (pPopupReturn->getEditBoxString(1) && *(pPopupReturn->getEditBoxString(1)))
			{
				szCivDescription = pPopupReturn->getEditBoxString(1);
			}
			if (pPopupReturn->getEditBoxString(2) && *(pPopupReturn->getEditBoxString(2)))
			{
				szCivShortDesc = pPopupReturn->getEditBoxString(2);
			}
			if (pPopupReturn->getEditBoxString(3) && *(pPopupReturn->getEditBoxString(3)))
			{
				szCivAdjective = pPopupReturn->getEditBoxString(3);
			}
			if (GC.getGame().isHotSeat() || GC.getGame().isPbem())
			{
				if (pPopupReturn->getEditBoxString(4) && *(pPopupReturn->getEditBoxString(4)))
				{
					szCivPassword = pPopupReturn->getEditBoxString(4);
				}
			}
			if (GC.getGame().isPitboss() || GC.getGame().isPbem())
			{
				if (pPopupReturn->getEditBoxString(5) && *(pPopupReturn->getEditBoxString(5)))
				{
					szEmail = CvString(pPopupReturn->getEditBoxString(5));
				}
			}
			if (GC.getGame().isPbem())
			{
				if (pPopupReturn->getEditBoxString(6) && *(pPopupReturn->getEditBoxString(6)))
				{
					szSmtpHost = CvString(pPopupReturn->getEditBoxString(6));
				}
			}

			GC.getInitCore().setLeaderName(eID, szLeaderName);
			GC.getInitCore().setCivDescription(eID, szCivDescription);
			GC.getInitCore().setCivShortDesc(eID, szCivShortDesc);
			GC.getInitCore().setCivAdjective(eID, szCivAdjective);
			if (szCivPassword != PASSWORD_DEFAULT)
			{
				GC.getInitCore().setCivPassword(eID, szCivPassword);
			}
			GC.getInitCore().setEmail(eID, szEmail);
			GC.getInitCore().setSmtpHost(eID, szSmtpHost);
			gDLL->sendPlayerInfo(eID);

			if (GC.getGame().isPbem() && pPopupReturn->getButtonClicked() == 0)
			{
				gDLL->sendPbemTurn(NO_PLAYER);
			}

		}
		break;

	case BUTTONPOPUP_ADMIN:
		{
			// Game details
			CvWString szGameName;
			CvWString szAdminPassword = GC.getInitCore().getAdminPassword();
			if (pPopupReturn->getEditBoxString(0) && *(pPopupReturn->getEditBoxString(0)))
			{
				szGameName = pPopupReturn->getEditBoxString(0);
			}
			if (pPopupReturn->getEditBoxString(1) && CvWString(pPopupReturn->getEditBoxString(1)) != PASSWORD_DEFAULT)
			{
				if (*(pPopupReturn->getEditBoxString(1)))
				{
					szAdminPassword = CvWString(gDLL->md5String((char*)CvString(pPopupReturn->getEditBoxString(1)).GetCString()));
				}
				else
				{
					szAdminPassword = L"";
				}
			}
			if (!GC.getGame().isGameMultiPlayer())
			{
				if (pPopupReturn->getCheckboxBitfield(2) > 0)
				{
					gDLL->setChtLvl(1);
				}
				else
				{
					gDLL->setChtLvl(0);
				}
			}

			gDLL->sendGameInfo(szGameName, szAdminPassword);

		}
		break;

	case BUTTONPOPUP_ADMIN_PASSWORD:
		{
			CvWString szAdminPassword;
			if (pPopupReturn->getEditBoxString(0) && CvWString(pPopupReturn->getEditBoxString(0)) != PASSWORD_DEFAULT)
			{
				szAdminPassword = pPopupReturn->getEditBoxString(0);
			}
			if (CvWString(gDLL->md5String((char*)CvString(szAdminPassword).GetCString())) == GC.getInitCore().getAdminPassword())
			{
				switch ((ControlTypes)info.getData1())
				{
				case CONTROL_WORLD_BUILDER:
					gDLL->getInterfaceIFace()->setWorldBuilder(!gDLL->GetWorldBuilderMode());
					break;
				case CONTROL_ADMIN_DETAILS:
					gDLL->getInterfaceIFace()->showAdminDetails();
					break;
				default:
					break;
				}
			}
			else
			{
				CvPopupInfo* pInfo = new CvPopupInfo();
				if (NULL != pInfo)
				{
					pInfo->setText(gDLL->getText("TXT_KEY_BAD_PASSWORD_DESC"));
					gDLL->getInterfaceIFace()->addPopup(pInfo, NO_PLAYER, true);
				}
			}
		}
		break;

	case BUTTONPOPUP_EXTENDED_GAME:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			if (GC.getGame().isNetworkMultiPlayer())
			{

				CvMessageControl::getInstance().sendExtendedGame();
			}
			else
			{
				GC.getGame().setGameState(GAMESTATE_EXTENDED);
			}
		}
		else if (pPopupReturn->getButtonClicked() == 1)
		{
			// exit to main menu
			if (GC.getGame().isNetworkMultiPlayer() && GC.getGame().canDoControl(CONTROL_RETIRE) && GC.getGame().countHumanPlayersAlive() > 1)
			{
				GC.getGame().doControl(CONTROL_RETIRE);
			}
			else if (!gDLL->getInterfaceIFace()->isDebugMenuCreated())
			{
				gDLL->getInterfaceIFace()->exitingToMainMenu();
			}
			else
			{
				gDLL->SetDone(true);
			}
		}
		break;

	case BUTTONPOPUP_DIPLOMACY:
		if (pPopupReturn->getButtonClicked() != MAX_PC_PLAYERS)
		{
			GET_PLAYER(GC.getGame().getActivePlayer()).contact((PlayerTypes)(pPopupReturn->getButtonClicked()));
		}
		break;

	case BUTTONPOPUP_ADDBUDDY:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			gDLL->AcceptBuddy(CvString(info.getText()).GetCString(), info.getData1());
		}
		else
		{
			gDLL->RejectBuddy(CvString(info.getText()).GetCString(), info.getData1());
		}
		break;

	case BUTTONPOPUP_FORCED_DISCONNECT:
	case BUTTONPOPUP_PITBOSS_DISCONNECT:
	case BUTTONPOPUP_KICKED:
		gDLL->getInterfaceIFace()->exitingToMainMenu();
		break;

	case BUTTONPOPUP_VASSAL_DEMAND_TRIBUTE:
		if (pPopupReturn->getButtonClicked() < GC.getNumBonusInfos())
		{
			PlayerTypes eVassal = (PlayerTypes)info.getData1();
			if (GET_PLAYER(eVassal).isHuman())
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_VASSAL_GRANT_TRIBUTE, GC.getGame().getActivePlayer(), pPopupReturn->getButtonClicked());
				if (NULL != pInfo)
				{
					gDLL->sendPopup(eVassal, pInfo);
				}
			}
			else
			{
				TradeData item;
				setTradeItem(&item, TRADE_RESOURCES, pPopupReturn->getButtonClicked());

				CLinkList<TradeData> ourList;
				CLinkList<TradeData> theirList;
				theirList.insertAtEnd(item);

				if (GET_PLAYER(eVassal).AI_considerOffer(GC.getGame().getActivePlayer(), &ourList, &theirList))
				{
					gDLL->sendImplementDealMessage(eVassal, &ourList, &theirList);


					CvWString szBuffer = gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_ACCEPTED", GET_PLAYER(eVassal).getNameKey(), GET_PLAYER(GC.getGame().getActivePlayer()).getNameKey(), GC.getBonusInfo((BonusTypes)pPopupReturn->getButtonClicked()).getTextKeyWide());
					AddDLLMessage(GC.getGame().getActivePlayer(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer);
				}
				else
				{
					CvMessageControl::getInstance().sendChangeWar(GET_PLAYER(eVassal).getTeam(), true);
				}
			}
		}
		break;

	case BUTTONPOPUP_VASSAL_GRANT_TRIBUTE:
		if (pPopupReturn->getButtonClicked() == 0)
		{
			TradeData item;
			setTradeItem(&item, TRADE_RESOURCES, info.getData2());

			CLinkList<TradeData> ourList;
			CLinkList<TradeData> theirList;
			ourList.insertAtEnd(item);

			gDLL->sendImplementDealMessage((PlayerTypes)info.getData1(), &ourList, &theirList);


			CvWString szBuffer = gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_ACCEPTED", GET_PLAYER(GC.getGame().getActivePlayer()).getNameKey(), GET_PLAYER((PlayerTypes)info.getData1()).getNameKey(), GC.getBonusInfo((BonusTypes)info.getData2()).getTextKeyWide());
			AddDLLMessage((PlayerTypes)info.getData1(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer);
		}
		else
		{
			CvMessageControl::getInstance().sendChangeWar(GET_PLAYER((PlayerTypes)info.getData1()).getTeam(), true);
		}

		break;

	case BUTTONPOPUP_EVENT:
		if (pPopupReturn->getButtonClicked() == GC.getNumEventInfos())
		{
			CvPlayer& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
			EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(info.getData1());
			if (NULL != pTriggeredData)
			{
				CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
				if (NULL != pCity)
				{
					gDLL->getInterfaceIFace()->selectCity(pCity, true);
				}
			}

			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EVENT, info.getData1());
			gDLL->getInterfaceIFace()->addPopup(pInfo, GC.getGame().getActivePlayer(), false, true);
		}
		else if (-1 != pPopupReturn->getButtonClicked())
		{
			CvMessageControl::getInstance().sendEventTriggered(GC.getGame().getActivePlayer(), (EventTypes)pPopupReturn->getButtonClicked(), info.getData1());
		}
		break;

	case BUTTONPOPUP_FREE_COLONY:
		if (pPopupReturn->getButtonClicked() > 0)
		{
			CvMessageControl::getInstance().sendEmpireSplit(GC.getGame().getActivePlayer(), pPopupReturn->getButtonClicked());
		}
		else if (pPopupReturn->getButtonClicked() < 0)
		{
			CvMessageControl::getInstance().sendDoTask(-pPopupReturn->getButtonClicked(), TASK_LIBERATE, 0, -1, false, false, false, false);
		}
		break;

	case BUTTONPOPUP_LAUNCH:
		if (0 == pPopupReturn->getButtonClicked())
		{
			CvMessageControl::getInstance().sendLaunch(GC.getGame().getActivePlayer(), (VictoryTypes)info.getData1());
		}
		break;

	case BUTTONPOPUP_FOUND_RELIGION:
		CvMessageControl::getInstance().sendFoundReligion(GC.getGame().getActivePlayer(), (ReligionTypes)pPopupReturn->getButtonClicked(), (ReligionTypes)info.getData1());
		break;

	case BUTTONPOPUP_GET_SAVE_FORMAT:
		{
			CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

			if (1 == pPopupReturn->getButtonClicked())
			{
				wrapper.m_requestUseTaggedFormat = true;
			}

			gDLL->SaveGame(SAVEGAME_NORMAL);
		}
		break;

	case BUTTONPOPUP_MODIFIER_RECALCULATION:
		{
			if (1 == pPopupReturn->getButtonClicked())
			{
				CvMessageControl::getInstance().sendRecalculateModifiers();;
			}
		}
		break;

	case BUTTONPOPUP_NAME_LIST:
		{
			if (pPopupReturn->getEditBoxString(0) && *(pPopupReturn->getEditBoxString(0)))
			{
				CvMessageControl::getInstance().sendBuildListEdit(info.getData1(), CvString(pPopupReturn->getEditBoxString(0)));
			}
		}
		break;
	case BUTTONPOPUP_INVASION:
		break;
	case BUTTONPOPUP_SELECT_UNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_SHADOW, info.getData2(), info.getData3(), pPopupReturn->getButtonClicked());
		}
		break;
	case BUTTONPOPUP_SELECT_DISCOVERY_TECH:
		if (pPopupReturn->getButtonClicked() != GC.getNumTechInfos())
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_WAIT_FOR_TECH, pPopupReturn->getButtonClicked(), pPopupReturn->getButtonClicked(), GC.getNumTechInfos());
		}
		break;
	case BUTTONPOPUP_CHOOSE_BUILDUP:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			CvMessageControl::getInstance().sendChooseBuildUp((PromotionLineTypes)pPopupReturn->getButtonClicked(), info.getData1());
		}
		else
		{
			CvMessageControl::getInstance().sendChooseBuildUp(NO_PROMOTIONLINE, info.getData1());
		}

		break;
	case BUTTONPOPUP_CHOOSE_TRAIT:
		if (GC.getTraitInfo((TraitTypes)pPopupReturn->getButtonClicked()).isNegativeTrait())
		{
			CvMessageControl::getInstance().sendChooseTrait((TraitTypes)pPopupReturn->getButtonClicked(), false);
			//GET_PLAYER(GC.getGame().getActivePlayer()).setHasTrait((TraitTypes)pPopupReturn->getButtonClicked(), false);
		}
		else
		{
			CvMessageControl::getInstance().sendChooseTrait((TraitTypes)pPopupReturn->getButtonClicked(), true);
			//GET_PLAYER(GC.getGame().getActivePlayer()).setHasTrait((TraitTypes)pPopupReturn->getButtonClicked(), true);
		}
		break;
	case BUTTONPOPUP_CHOOSE_TRAIT_NEGATIVE:
		if (GC.getTraitInfo((TraitTypes)pPopupReturn->getButtonClicked()).isNegativeTrait())
		{
			CvMessageControl::getInstance().sendChooseTrait((TraitTypes)pPopupReturn->getButtonClicked(), true);
		}
		else
		{
			CvMessageControl::getInstance().sendChooseTrait((TraitTypes)pPopupReturn->getButtonClicked(), false);
		}
		//GET_PLAYER(GC.getGame().getActivePlayer()).setHasTrait((TraitTypes)pPopupReturn->getButtonClicked(), true);
		break;

	case BUTTONPOPUP_CHOOSE_MERGE_UNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			CvMessageControl::getInstance().sendMergeUnit(pPopupReturn->getButtonClicked());
		}
		break;

	case BUTTONPOPUP_CONFIRM_SPLIT_UNIT:
		if (pPopupReturn->getButtonClicked() == 1)
		{
			CvMessageControl::getInstance().sendSplitUnit(true);
		}
		else
		{
			CvMessageControl::getInstance().sendSplitUnit(false);
		}
		break;

	case BUTTONPOPUP_IMPROVEMENT_UPGRADE_OPTIONS:
		if (pPopupReturn->getButtonClicked() != GC.getNumImprovementInfos())
		{
			CvMessageControl::getInstance().sendImprovementUpgrade((ImprovementTypes)pPopupReturn->getButtonClicked(), info.getData2(), info.getData3(), true);
		}
		else
		{
			CvMessageControl::getInstance().sendImprovementUpgrade(NO_IMPROVEMENT, info.getData2(), info.getData3(), false);
		}
		break;

	case BUTTONPOPUP_GOTO_CITY:
		break;


	case BUTTONPOPUP_CHOOSE_ARREST_UNIT:
		if (pPopupReturn->getButtonClicked() != 0)
		{
			int iUnitID = pPopupReturn->getButtonClicked();
			CvPlot* pPlot = GC.getMap().plot(info.getData2(), info.getData3());
			bst::optional<CvUnit*> clickedUnit = algo::find_if(pPlot->units(), CvUnit::fn::getID() == iUnitID);
			if (clickedUnit)
			{
				CvMessageControl::getInstance().sendArrestUnit(iUnitID, (*clickedUnit)->getOwner());
			}
		}
		else
		{
			GET_PLAYER(GC.getGame().getActivePlayer()).setArrestingUnit(FFreeList::INVALID_INDEX);
		}
		break;

	case BUTTONPOPUP_CONFIRM_AMBUSH:
		if (pPopupReturn->getButtonClicked() == 1)
		{
			CvMessageControl::getInstance().sendAmbushConfirmation(true);
		}
		else
		{
			CvMessageControl::getInstance().sendAmbushConfirmation(false);
			GET_PLAYER(GC.getGame().getActivePlayer()).setAmbushingUnit(FFreeList::INVALID_INDEX);
		}
		break;

	default:
		FErrorMsg("error");
		break;
	}
}

void CvDLLButtonPopup::OnFocus(CvPopup* pPopup, CvPopupInfo &info)
{
	if (gDLL->getInterfaceIFace()->popupIsDying(pPopup))
	{
		return;
	}

	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	FAssert(GET_PLAYER(ePlayer).isHuman());

	switch (info.getButtonPopupType())
	{
	case BUTTONPOPUP_CHOOSETECH:
		if (info.getData1() == 0)
		{
			if ((GET_PLAYER(GC.getGame().getActivePlayer()).getCurrentResearch() != NO_TECH) || (GC.getGame().getGameState() == GAMESTATE_OVER))
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
			}
		}
		break;

	case BUTTONPOPUP_CHANGERELIGION:
		if (!(GET_PLAYER(GC.getGame().getActivePlayer()).canChangeReligion()) || (GC.getGame().getGameState() == GAMESTATE_OVER))
		{
			gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
		}
		break;

	case BUTTONPOPUP_CHOOSEPRODUCTION:
		if (GC.getGame().getGameState() == GAMESTATE_OVER)
		{
			gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
		}
		else
		{
			CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());

			if (NULL == pCity || pCity->getOwner() != ePlayer || pCity->isProduction())
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
				break;
			}

			if (getBugOptionBOOL("RoMSettings__NoProductionPopup", false, "ZOOM_CITY_FOR_PRODUCTION_POPUP"))
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
				gDLL->getInterfaceIFace()->selectCity(pCity);
				break;
			}

			if (GC.getCurrentViewport()->isInViewport(pCity->getX(), pCity->getY(), GC.getVIEWPORT_FOCUS_BORDER()))
			{
				gDLL->getInterfaceIFace()->lookAtCityOffset(pCity->getID());
			}
			else
			{
				GC.getCurrentViewport()->bringIntoView(pCity->getX(), pCity->getY(), NULL, true);
			}
		}
		break;

	case BUTTONPOPUP_RAZECITY:
	case BUTTONPOPUP_DISBANDCITY:
		{
			const CvCity* pCity = GET_PLAYER(ePlayer).getCity(info.getData1());

			if (NULL == pCity || pCity->getOwner() != ePlayer)
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
				break;
			}

			if (GC.getCurrentViewport()->isInViewport(pCity->getX(), pCity->getY(), GC.getVIEWPORT_FOCUS_BORDER()))
			{
				gDLL->getInterfaceIFace()->lookAtCityOffset(pCity->getID());
			}
			else
			{
				GC.getCurrentViewport()->bringIntoView(pCity->getX(), pCity->getY(), NULL, true);
			}
		}
		break;

	case BUTTONPOPUP_CHANGECIVIC:
		if (!(GET_PLAYER(GC.getGame().getActivePlayer()).canRevolution(NULL)) || (GC.getGame().getGameState() == GAMESTATE_OVER))
		{
			gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
		}
		break;

	case BUTTONPOPUP_PYTHON:
	case BUTTONPOPUP_PYTHON_SCREEN:
		if (!info.getOnFocusPythonCallback().IsEmpty())
		{
			if (Cy::call<bool>(PYScreensModule, info.getOnFocusPythonCallback(), Cy::Args()
				<< info.getData1()
				<< info.getData2()
				<< info.getData3()
				<< info.getFlags()
				<< info.getText()
				<< info.getOption1()
				<< info.getOption2()
				))
			{
				gDLL->getInterfaceIFace()->popupSetAsCancelled(pPopup);
			}
		}
		break;

	}
}

// returns false if popup is not launched
bool CvDLLButtonPopup::launchButtonPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	GET_PLAYER(GC.getGame().getActivePlayer()).setTurnHadUIInteraction(true);

	switch (info.getButtonPopupType())
	{
		case BUTTONPOPUP_TEXT:
		{
			return launchTextPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSEPRODUCTION:
		{
			return launchProductionPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHANGERELIGION:
		{
			return launchChangeReligionPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSEELECTION:
		{
			return launchChooseElectionPopup(pPopup, info);
		}
		case BUTTONPOPUP_DIPLOVOTE:
		{
			return launchDiploVotePopup(pPopup, info);
		}
		case BUTTONPOPUP_RAZECITY:
		{
			return launchRazeCityPopup(pPopup, info);
		}
		case BUTTONPOPUP_DISBANDCITY:
		{
			return launchDisbandCityPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSETECH:
		{
			return launchChooseTechPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHANGECIVIC:
		{
			return launchChangeCivicsPopup(pPopup, info);
		}
		case BUTTONPOPUP_ALARM:
		{
			return launchAlarmPopup(pPopup, info);
		}
		case BUTTONPOPUP_DECLAREWARMOVE:
		{
			return launchDeclareWarMovePopup(pPopup, info);
		}
		case BUTTONPOPUP_CONFIRMCOMMAND:
		{
			return launchConfirmCommandPopup(pPopup, info);
		}
		case BUTTONPOPUP_LOADUNIT:
		{
			return launchLoadUnitPopup(pPopup, info);
		}
		case BUTTONPOPUP_LEADUNIT:
		{
			return launchLeadUnitPopup(pPopup, info);
		}
		case BUTTONPOPUP_DOESPIONAGE:
		{
			return launchDoEspionagePopup(pPopup, info);
		}
		case BUTTONPOPUP_DOESPIONAGE_TARGET:
		{
			return launchDoEspionageTargetPopup(pPopup, info);
		}
		case BUTTONPOPUP_MAIN_MENU:
		{
			return launchMainMenuPopup(pPopup, info);
		}
		case BUTTONPOPUP_CONFIRM_MENU:
		{
			return launchConfirmMenu(pPopup, info);
		}
		case BUTTONPOPUP_PYTHON_SCREEN:
		{
			return launchPythonScreen(pPopup, info);
		}
		case BUTTONPOPUP_DEAL_CANCELED:
		{
			return launchCancelDeal(pPopup, info);
		}
		case BUTTONPOPUP_PYTHON:
		{
			return launchPythonPopup(pPopup, info);
		}
		case BUTTONPOPUP_DETAILS:
		{
			return launchDetailsPopup(pPopup, info);
		}
		case BUTTONPOPUP_ADMIN:
		{
			return launchAdminPopup(pPopup, info);
		}
		case BUTTONPOPUP_ADMIN_PASSWORD:
		{
			return launchAdminPasswordPopup(pPopup, info);
		}
		case BUTTONPOPUP_EXTENDED_GAME:
		{
			return launchExtendedGamePopup(pPopup, info);
		}
		case BUTTONPOPUP_DIPLOMACY:
		{
			return launchDiplomacyPopup(pPopup, info);
		}
		case BUTTONPOPUP_ADDBUDDY:
		{
			return launchAddBuddyPopup(pPopup, info);
		}
		case BUTTONPOPUP_FORCED_DISCONNECT:
		{
			return launchForcedDisconnectPopup(pPopup, info);
		}
		case BUTTONPOPUP_PITBOSS_DISCONNECT:
		{
			return launchPitbossDisconnectPopup(pPopup, info);
		}
		case BUTTONPOPUP_KICKED:
		{
			return launchKickedPopup(pPopup, info);
		}
		case BUTTONPOPUP_VASSAL_DEMAND_TRIBUTE:
		{
			return launchVassalDemandTributePopup(pPopup, info);
		}
		case BUTTONPOPUP_VASSAL_GRANT_TRIBUTE:
		{
			return launchVassalGrantTributePopup(pPopup, info);
		}
		case BUTTONPOPUP_EVENT:
		{
			return launchEventPopup(pPopup, info);
		}
		case BUTTONPOPUP_FREE_COLONY:
		{
			return launchFreeColonyPopup(pPopup, info);
		}
		case BUTTONPOPUP_LAUNCH:
		{
			return launchLaunchPopup(pPopup, info);
		}
		case BUTTONPOPUP_FOUND_RELIGION:
		{
			return launchFoundReligionPopup(pPopup, info);
		}
		case BUTTONPOPUP_GET_SAVE_FORMAT:
		{
			return launchGetSaveFormatPopup(pPopup, info);
		}
		case BUTTONPOPUP_MODIFIER_RECALCULATION:
		{
			return launchModifierRecalculationPopup(pPopup, info);
		}
		case BUTTONPOPUP_NAME_LIST:
		{
			return launchNameListPopup(pPopup, info);
		}
		case BUTTONPOPUP_INVASION:
		{
			return invasionPopup(pPopup, info);
		}
		case BUTTONPOPUP_SELECT_UNIT:
		{
			return launchSelectShadowUnitPopup(pPopup, info);
		}
		case BUTTONPOPUP_SELECT_DISCOVERY_TECH:
		{
			return launchSelectDiscoveryTechPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSE_BUILDUP:
		{
			return launchChooseBuildUpPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSE_TRAIT:
		{
			return launchChooseTraitPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSE_TRAIT_NEGATIVE:
		{
			return launchChooseTraitPopupNegative(pPopup, info);
		}
		case BUTTONPOPUP_GOTO_CITY:
		{
			return launchGoToCityPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSE_MERGE_UNIT:
		{
			return launchSelectMergeUnitPopup(pPopup, info);
		}
		case BUTTONPOPUP_CONFIRM_SPLIT_UNIT:
		{
			return launchConfirmSplitUnitPopup(pPopup, info);
		}
		case BUTTONPOPUP_IMPROVEMENT_UPGRADE_OPTIONS:
		{
			return launchImprovementUpgradeOptionsPopup(pPopup, info);
		}
		case BUTTONPOPUP_CHOOSE_ARREST_UNIT:
		{
			return launchSelectArrestUnitPopup(pPopup, info);
		}
		case BUTTONPOPUP_CONFIRM_AMBUSH:
		{
			return launchConfirmAmbushPopup(pPopup, info);
		}
		default: FErrorMsg("error");
	}
	return false;
}


bool CvDLLButtonPopup::launchTextPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, info.getText());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}

// Helper container for use in launchProductionPopup for sorting unit build options
struct UnitBuildItem
{
	UnitTypes type;
	int strength;

	UnitBuildItem(UnitTypes type = NO_UNIT, int strength = 0) : type(type), strength(strength) {}
	bool operator<(const UnitBuildItem& other) const { return strength < other.strength; }
};

// Helper container for use in launchProductionPopup for sorting project build options
struct ProjectBuildItem
{
	ProjectTypes type;
	int turns;

	ProjectBuildItem(ProjectTypes type = NO_PROJECT, int turns = 0) : type(type), turns(turns) {}
	bool operator<(const ProjectBuildItem& other) const { return turns < other.turns; }
};

bool CvDLLButtonPopup::launchProductionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());
	if (NULL == pCity || pCity->isProductionAutomated())
	{
		return (false);
	}
	FAssertMsg(pCity->getOwner() == GC.getGame().getActivePlayer(), "City must belong to Active Player");

	// This popup might be called to suggest a specific thing that the player should do, as opposed to just
	// indicating that a build queue is empty. In this case the thing to build is determined below
	UnitTypes eTrainUnit = NO_UNIT;
	BuildingTypes eConstructBuilding = NO_BUILDING;
	ProjectTypes eCreateProject = NO_PROJECT;
	switch (info.getData2())
	{
	case (ORDER_TRAIN):
		eTrainUnit = (UnitTypes)info.getData3();
		break;
	case (ORDER_CONSTRUCT):
		eConstructBuilding = (BuildingTypes)info.getData3();
		break;
	case (ORDER_CREATE):
		eCreateProject = (ProjectTypes)info.getData3();
		break;
	default:
		break;
	}
	bool bFinish = info.getOption1();

	CvWString szPopupHeader;
	CvString szArtFilename;
	if (eTrainUnit != NO_UNIT)
	{
		if (bFinish)
		{
			szPopupHeader = gDLL->getText((isLimitedUnit(eTrainUnit) ? "TXT_KEY_POPUP_TRAINED_WORK_ON_NEXT_LIMITED" : "TXT_KEY_POPUP_TRAINED_WORK_ON_NEXT"), GC.getUnitInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey());
		}
		else
		{
			szPopupHeader = gDLL->getText((isLimitedUnit(eTrainUnit) ? "TXT_KEY_POPUP_CANNOT_TRAIN_WORK_NEXT_LIMITED" : "TXT_KEY_POPUP_CANNOT_TRAIN_WORK_NEXT"), GC.getUnitInfo(eTrainUnit).getTextKeyWide(), pCity->getNameKey());
		}
		szArtFilename = GET_PLAYER(pCity->getOwner()).getUnitButton(eTrainUnit);
	}
	else if (eConstructBuilding != NO_BUILDING)
	{
		if (bFinish)
		{
			szPopupHeader = gDLL->getText(isLimitedWonder(eConstructBuilding) ? "TXT_KEY_POPUP_CONSTRUCTED_WORK_ON_NEXT_LIMITED" : "TXT_KEY_POPUP_CONSTRUCTED_WORK_ON_NEXT", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey());
		}
		else
		{
			szPopupHeader = gDLL->getText(isLimitedWonder(eConstructBuilding) ? "TXT_KEY_POPUP_CANNOT_CONSTRUCT_WORK_NEXT_LIMITED" : "TXT_KEY_POPUP_CANNOT_CONSTRUCT_WORK_NEXT", GC.getBuildingInfo(eConstructBuilding).getTextKeyWide(), pCity->getNameKey());
		}
		szArtFilename = GC.getBuildingInfo(eConstructBuilding).getButton();
	}
	else if (eCreateProject != NO_PROJECT)
	{
		if (bFinish)
		{
			if(GC.getProjectInfo(eCreateProject).isSpaceship())
				szPopupHeader = gDLL->getText("TXT_KEY_POPUP_CREATED_WORK_ON_NEXT_SPACESHIP", GC.getProjectInfo(eCreateProject).getTextKeyWide(), pCity->getNameKey());
			else
				szPopupHeader = gDLL->getText(((isLimitedProject(eCreateProject)) ? "TXT_KEY_POPUP_CREATED_WORK_ON_NEXT_LIMITED" : "TXT_KEY_POPUP_CREATED_WORK_ON_NEXT"), GC.getProjectInfo(eCreateProject).getTextKeyWide(), pCity->getNameKey());
		}
		else
		{
			szPopupHeader = gDLL->getText(((isLimitedProject(eCreateProject)) ? "TXT_KEY_POPUP_CANNOT_CREATE_WORK_NEXT_LIMITED" : "TXT_KEY_POPUP_CANNOT_CREATE_WORK_NEXT"), GC.getProjectInfo(eCreateProject).getTextKeyWide(), pCity->getNameKey());
		}
		szArtFilename = GC.getProjectInfo(eCreateProject).getButton();
	}
	else
	{
		szPopupHeader = gDLL->getText("TXT_KEY_POPUP_WHAT_TO_BUILD", pCity->getNameKey());
		szArtFilename = ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_PRODUCTION")->getPath();
	}

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, szPopupHeader, DLL_FONT_LEFT_JUSTIFY);

	int iExamineCityID = std::max(0, GC.getNumUnitInfos());
	iExamineCityID = std::max(iExamineCityID, GC.getNumBuildingInfos());
	iExamineCityID = std::max(iExamineCityID, GC.getNumProjectInfos());
	iExamineCityID = std::max(iExamineCityID, GC.getNumProcessInfos());

	if (getBugOptionBOOL("MiscHover__CDAZoomCityDetails", true, "BUG_CDA_ZOOM_CITY_DETAILS"))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), iExamineCityID, WIDGET_ZOOM_CITY, GC.getGame().getActivePlayer(), info.getData1(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
	}
	else
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), iExamineCityID, WIDGET_GENERAL, -1, -1, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
	}
	ProjectTypes eProductionProject = pCity->getProductionProject();
	ProcessTypes eProductionProcess = pCity->getProductionProcess();

	// If we aren't already suggesting a unit to the player then get one from the advisers
	int iDummyValue;
	const UnitTypes eProductionUnit = pCity->AI_bestUnit(iDummyValue, -1, NULL, true, NULL, false, false, NULL);

	int iNumBuilds = 0;
	if (eProductionUnit != NO_UNIT)
	{
		const AdvisorTypes eUnitAdvisor = (AdvisorTypes)GC.getUnitInfo(eProductionUnit).getAdvisorType();
		if (eUnitAdvisor != NO_ADVISOR)
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(
				pPopup,
				gDLL->getText(
					"TXT_KEY_POPUP_RECOMMENDED", GC.getUnitInfo(eProductionUnit).getTextKeyWide(),
					pCity->getProductionTurnsLeft(eProductionUnit, 0), GC.getAdvisorInfo(eUnitAdvisor).getTextKeyWide()
				),
				GET_PLAYER(pCity->getOwner()).getUnitButton(eProductionUnit), eProductionUnit, WIDGET_TRAIN,
				eProductionUnit, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY
			);
			iNumBuilds++;
		}
		else // just in case we have units with no defined advisor type.
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(
				pPopup,
				gDLL->getText(
					"TXT_KEY_POPUP_RECOMMENDED_NO_ADV",
					GC.getUnitInfo(eProductionUnit).getTextKeyWide(), pCity->getProductionTurnsLeft(eProductionUnit, 0)
				),
				GET_PLAYER(pCity->getOwner()).getUnitButton(eProductionUnit), eProductionUnit, WIDGET_TRAIN,
				eProductionUnit, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY
			);
			iNumBuilds++;
		}
	}

	// === BUILDINGS =========================================================
	std::vector<BuildingTypes> possibleBuildings;
	for (int idx = 0; idx < GC.getNumBuildingInfos(); idx++)
	{
		const BuildingTypes building = static_cast<BuildingTypes>(idx);

		// Make sure to exclude Palace from the recommended list (it is the only one with isCaptial)!
		if (pCity->canConstruct(building) && !GC.getBuildingInfo(building).isCapital())
		{
			possibleBuildings.push_back(building);
		}
	}

	std::vector<CvCity::ScoredBuilding> bestBuildings;
	if (pCity->AI_scoreBuildingsFromListThreshold(bestBuildings, possibleBuildings, 0, 50, 0, true))
	{
		// Work out statistics about the spread of the building scores so we can see if any are highly recommended
		float average;
		int64_t minScore, maxScore;
		CvCity::ScoredBuilding::averageMinMax(bestBuildings, average, minScore, maxScore);

		bestBuildings.resize(std::min<int>(5, bestBuildings.size()));

		const float cutOff = average + (maxScore - average) * 0.75f;
		for(size_t idx = 0; idx < bestBuildings.size(); ++idx)
		{
			const BuildingTypes building = bestBuildings[idx].building;
			const AdvisorTypes advisor = (AdvisorTypes)GC.getBuildingInfo(building).getAdvisorType();
			if (bestBuildings[idx].score > cutOff && advisor != NO_ADVISOR)
			{
				const int iTurns = pCity->getProductionTurnsLeft(building, 0);
				const CvWString szBuildingText = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED", GC.getBuildingInfo(building).getTextKeyWide(), iTurns, GC.getAdvisorInfo(advisor).getTextKeyWide());
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuildingText, GC.getBuildingInfo(building).getButton(), building, WIDGET_CONSTRUCT, building, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			}
			else
			{
				const int iTurns = pCity->getProductionTurnsLeft(building, 0);
				const CvWString szBuildingText = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED_NO_ADV", GC.getBuildingInfo(building).getTextKeyWide(), iTurns);
				// CvWString szBuildingText = CvWString::format(L"%s (%d)", GC.getBuildingInfo(building).getDescription(), iTurns);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuildingText, GC.getBuildingInfo(building).getButton(), building, WIDGET_CONSTRUCT, building, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			}
			iNumBuilds++;
		}
	}

	if (eProductionProject != NO_PROJECT)
	{
		int iTurns = pCity->getProductionTurnsLeft(eProductionProject, 0);
		CvWString svProjectText = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED_NO_ADV", GC.getProjectInfo(eProductionProject).getTextKeyWide(), iTurns);
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, svProjectText, GC.getProjectInfo(eProductionProject).getButton(), eProductionProject, WIDGET_CREATE, eProductionProject, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY );
		iNumBuilds++;
	}

	if (eProductionProcess != NO_PROCESS)
	{
		CvWString szProcessText = gDLL->getText("TXT_KEY_POPUP_RECOMMENDED_NO_ADV_OR_TURNS", GC.getProcessInfo(eProductionProcess).getTextKeyWide());
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szProcessText, GC.getProcessInfo(eProductionProcess).getButton(), eProductionProcess, WIDGET_MAINTAIN, eProductionProcess, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY );
		iNumBuilds++;
	}

	// === UNITS =========================================================
	//TB SubCombat Mod Footnote:
	// This following section is the only place in the dll files that references getUnitCombatType that has not been updated for SubCombats as I'm not sure where it displays nor how to best frame it yet.
	{
		// Add the 5 strongest military units
		std::vector<UnitBuildItem> units;

		// Start with a few useful units
		UnitTypes bestWorker, bestSettler;
		{
			int bestValue;
			CvUnitSelectionCriteria criteria = CvUnitSelectionCriteria().UnitAI(UNITAI_WORKER).IgnoreGrowth(true);
			bestWorker = pCity->AI_bestUnitAI(UNITAI_WORKER, bestValue, true, true, &criteria);

			if (bestWorker != NO_UNIT && bestWorker != eProductionUnit)
			{
				units.push_back(UnitBuildItem(bestWorker, MAX_INT));
			}
		}
		{
			int bestValue;
			CvUnitSelectionCriteria criteria = CvUnitSelectionCriteria().UnitAI(UNITAI_SETTLE).IgnoreGrowth(true);
			bestSettler = pCity->AI_bestUnitAI(UNITAI_SETTLE, bestValue, true, true, &criteria);

			if (bestSettler != NO_UNIT && bestSettler != eProductionUnit)
			{
				units.push_back(UnitBuildItem(bestSettler, MAX_INT - 1));
			}
		}
		// Then do a general sweep
		for (int iI = 0; iI < GC.getNumUnitInfos(); ++iI)
		{
			UnitTypes eUnit = (UnitTypes) iI;
			if (eUnit != eProductionUnit && eUnit != bestWorker && eUnit != bestSettler && pCity->canTrain(eUnit))
			{
				units.push_back(UnitBuildItem(eUnit, GC.getUnitInfo(eUnit).getTotalModifiedCombatStrength100()));
			}
		}

		// Sort the units from strongest to weakest
		std::sort(units.rbegin(), units.rend());

		// Max 5 units shown
		units.resize(std::min<int>(units.size(), 5));

		for (size_t unitIdx = 0; unitIdx < units.size(); unitIdx++)
		{
			const UnitBuildItem& unitBuildItem = units[unitIdx];
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup,
				CvWString::format(L"%s (%d)", GC.getUnitInfo(unitBuildItem.type).getDescription(), pCity->getProductionTurnsLeft(unitBuildItem.type, 0)),
				GET_PLAYER(pCity->getOwner()).getUnitButton(unitBuildItem.type),
				unitBuildItem.type, WIDGET_TRAIN, unitBuildItem.type, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			iNumBuilds++;
		}
	}
	// === PROJECTS =========================================================

	// List any projects available by turns to complete
	{
		std::vector<ProjectBuildItem> projects;

		for (int projectIdx = 0; projectIdx < GC.getNumProjectInfos(); projectIdx++)
		{
			if (projectIdx != eProductionProject && pCity->canCreate((ProjectTypes)projectIdx))
			{
				projects.push_back(ProjectBuildItem((ProjectTypes)projectIdx, pCity->getProductionTurnsLeft((ProjectTypes)projectIdx, 0)));
			}
		}

		// Sort the projects by turns to complete
		algo::sort(projects);

		// Lets only keep 5 (probably there will never be this many projects)
		projects.resize(std::min<int>(5, projects.size()));

		// Retrieve the Index and generate the list
		for (size_t projectIdx = 0; projectIdx < projects.size(); projectIdx++)
		{
			const ProjectBuildItem& projectBuildItem = projects[projectIdx];

			// Retrieve the Index
			if (projectBuildItem.type != eProductionProject && pCity->canCreate((ProjectTypes)projectBuildItem.type))
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString::format(L"%s (%d)", GC.getProjectInfo(projectBuildItem.type).getDescription(), projectBuildItem.turns), GC.getProjectInfo(projectBuildItem.type).getButton(), projectBuildItem.type, WIDGET_CREATE, projectBuildItem.type, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
				iNumBuilds++;
			}
		}
	}

	// === PROCESSES =========================================================
	for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		if (iI != eProductionProcess && pCity->canMaintain((ProcessTypes)iI))
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getProcessInfo((ProcessTypes) iI).getDescription(), GC.getProcessInfo((ProcessTypes) iI).getButton(), iI, WIDGET_MAINTAIN, iI, pCity->getID(), true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
			iNumBuilds++;
		}
	}

	if (0 == iNumBuilds)
	{
		// city cannot build anything, so don't show popup after all
		return (false);
	}

	gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_PRODUCTION, szArtFilename);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED, 252);

	switch (info.getData2())
	{
	case ORDER_TRAIN:
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getUnitInfo((UnitTypes)info.getData3()).getArtInfo(0, GET_PLAYER(pCity->getOwner()).getCurrentEra(), NO_UNIT_ARTSTYLE)->getTrainSound());
		break;

	case ORDER_CONSTRUCT:
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getBuildingInfo((BuildingTypes)info.getData3()).getConstructSound());
		break;

	case ORDER_CREATE:
		gDLL->getInterfaceIFace()->playGeneralSound(GC.getProjectInfo((ProjectTypes)info.getData3()).getCreateSound());
		break;

	default:
		break;
	}
	return (true);
}

bool CvDLLButtonPopup::launchChangeReligionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvWString szTemp;
	ReligionTypes eReligion = (ReligionTypes)info.getData1();

	if (NO_RELIGION == eReligion)
	{
		FErrorMsg("error");
		return (false);
	}

	CvPlayer& activePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	if (!activePlayer.canConvert(eReligion))
	{
		return (false);
	}

	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_POPUP_RELIGION_SPREAD", GC.getReligionInfo(eReligion).getTextKeyWide());
	if (activePlayer.getStateReligionHappiness() != 0)
	{
		if (activePlayer.getStateReligionHappiness() > 0)
		{
			szBuffer += gDLL->getText("TXT_KEY_POPUP_CONVERTING_EFFECTS",
				activePlayer.getStateReligionHappiness(), gDLL->getSymbolID(HAPPY_CHAR), GC.getReligionInfo(eReligion).getChar());
		}
		else
		{
			szBuffer += gDLL->getText("TXT_KEY_POPUP_CONVERTING_EFFECTS",
				-activePlayer.getStateReligionHappiness(), gDLL->getSymbolID(UNHAPPY_CHAR), GC.getReligionInfo(eReligion).getChar());
		}
	}
	szBuffer += gDLL->getText("TXT_KEY_POPUP_LIKE_TO_CONVERT");
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);

	szBuffer = gDLL->getText("TXT_KEY_POPUP_CONVERT_RELIGION");
	int iAnarchyLength = activePlayer.getReligionAnarchyLength();
	if (iAnarchyLength > 0)
	{
		szBuffer += gDLL->getText("TXT_KEY_POPUP_TURNS_OF_ANARCHY", iAnarchyLength);
	}
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, NULL, 0);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO_CONVERSION").c_str());
	gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_RELIGION, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_RELIGION")->getPath());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED);
	return (true);
}


bool CvDLLButtonPopup::launchChooseElectionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	VoteSelectionData* pVoteSelectionData = GC.getGame().getVoteSelection(info.getData1());
	if (NULL == pVoteSelectionData)
	{
		return false;
	}

	VoteSourceTypes eVoteSource = pVoteSelectionData->eVoteSource;

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, GC.getVoteSourceInfo(eVoteSource).getPopupText());

	for (int iI = 0; iI < (int)pVoteSelectionData->aVoteOptions.size(); iI++)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, pVoteSelectionData->aVoteOptions[iI].szText, NULL, iI, WIDGET_GENERAL);
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NONE").c_str(), NULL, GC.getNumVoteInfos(), WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchDiploVotePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	VoteTriggeredData* pVoteTriggered = GC.getGame().getVoteTriggered(info.getData1());
	if (NULL == pVoteTriggered)
	{
		FErrorMsg("error");
		return false;
	}

	VoteTypes eVote = pVoteTriggered->kVoteOption.eVote;
	VoteSourceTypes eVoteSource = pVoteTriggered->eVoteSource;

	TeamTypes eVassalOfTeam = NO_TEAM;
	bool bEligible = false;

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, GC.getVoteSourceInfo(eVoteSource).getDescription());
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, pVoteTriggered->kVoteOption.szText);
	if (GC.getGame().isTeamVote(eVote))
	{
		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GC.getGame().isTeamVoteEligible((TeamTypes)iI, eVoteSource))
				{
					if (GET_TEAM(GC.getGame().getActiveTeam()).isVassal((TeamTypes)iI))
					{
						eVassalOfTeam = (TeamTypes)iI;
						break;
					}
				}
			}
		}

		for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GC.getGame().isTeamVoteEligible((TeamTypes)iI, eVoteSource))
				{
					if (eVassalOfTeam == NO_TEAM || eVassalOfTeam == iI || iI == GC.getGame().getActiveTeam())
					{
						gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GET_TEAM((TeamTypes)iI).getName().GetCString(), NULL, iI, WIDGET_GENERAL);
						bEligible = true;
					}
				}
			}
		}
	}
	else
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, PLAYER_VOTE_YES, WIDGET_GENERAL);
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str(), NULL, PLAYER_VOTE_NO, WIDGET_GENERAL);

		if (GET_PLAYER(GC.getGame().getActivePlayer()).canDefyResolution(eVoteSource, pVoteTriggered->kVoteOption))
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_VOTE_NEVER").c_str(), NULL, PLAYER_VOTE_NEVER, WIDGET_GENERAL);
		}
		bEligible = true;
	}

	if (eVassalOfTeam == NO_TEAM || !bEligible)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_ABSTAIN").c_str(), NULL, PLAYER_VOTE_ABSTAIN, WIDGET_GENERAL);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchRazeCityPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& player = GET_PLAYER(GC.getGame().getActivePlayer());

	CvCity* pNewCity = player.getCity(info.getData1());
	if (NULL == pNewCity)
	{
		FErrorMsg("error");
		return (false);
	}

	if (0 != GC.getPLAYER_ALWAYS_RAZES_CITIES())
	{
		player.raze(pNewCity);
		return false;
	}

	PlayerTypes eHighestCulturePlayer = (PlayerTypes)info.getData2();

	int iCaptureGold = info.getData3();
	bool bRaze = player.canRaze(pNewCity);
	bool bGift = ((eHighestCulturePlayer != NO_PLAYER)
		&& (eHighestCulturePlayer != player.getID())
		&& ((player.getTeam() == GET_PLAYER(eHighestCulturePlayer).getTeam()) || GET_TEAM(player.getTeam()).isOpenBorders(GET_PLAYER(eHighestCulturePlayer).getTeam()) || GET_TEAM(GET_PLAYER(eHighestCulturePlayer).getTeam()).isVassal(player.getTeam())));

	CvWString szBuffer;
	if (iCaptureGold > 0)
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_GOLD_CITY_CAPTURE", iCaptureGold, pNewCity->getNameKey());
	}
	else
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_CITY_CAPTURE_KEEP", pNewCity->getNameKey());
	}
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_KEEP_CAPTURED_CITY").c_str(), NULL, 0, WIDGET_GENERAL);

	if (bRaze)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_RAZE_CAPTURED_CITY").c_str(), NULL, 1, WIDGET_GENERAL);
	}
	if (bGift)
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_RETURN_ALLIED_CITY", GET_PLAYER(eHighestCulturePlayer).getCivilizationDescriptionKey());
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, NULL, 2, WIDGET_GENERAL, 2, eHighestCulturePlayer);
	}
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_CITY_WARNING_ANSWER3").c_str(), NULL, 3, WIDGET_GENERAL, -1, -1);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	gDLL->getInterfaceIFace()->playGeneralSound("AS2D_CITYCAPTURE");

	return (true);
}

bool CvDLLButtonPopup::launchDisbandCityPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& player = GET_PLAYER(GC.getGame().getActivePlayer());

	CvCity* pNewCity = player.getCity(info.getData1());
	if (NULL == pNewCity)
	{
		FErrorMsg("error");
		return (false);
	}

	CvWString szBuffer;
	szBuffer = gDLL->getText("TXT_KEY_POPUP_FLIPPED_CITY_KEEP", pNewCity->getNameKey());
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_KEEP_FLIPPED_CITY").c_str(), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_DISBAND_FLIPPED_CITY").c_str(), NULL, 1, WIDGET_GENERAL);
// BUG - Examine Culture Flip - start
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_CITY_WARNING_ANSWER3").c_str(), NULL, 2, WIDGET_GENERAL, -1, -1);
// BUG - Examine Culture Flip - end
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	gDLL->getInterfaceIFace()->playGeneralSound("AS2D_CULTUREFLIP");

	return (true);
}
//ls612: City Goto in Viewports
bool CvDLLButtonPopup::launchGoToCityPopup(CvPopup *pPopup, CvPopupInfo &info)
{
	const CvPlayer& kPlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	CvWString szBuffer = gDLL->getText("TXT_KEY_POPUP_GOTO_CITY");
	const CvUnit* pUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
	if ( pUnit != NULL )
	{
		switch (pUnit->getDomainType())
		{
		case DOMAIN_LAND: {

			gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);

			int buttonId = 0;
			foreach_(const CvCity* pCity, kPlayer.cities())
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText(pCity->getName()).c_str(), NULL, buttonId, WIDGET_CITY_GOTO, pCity->getID());
				buttonId++;
			}

			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, buttonId, WIDGET_GENERAL);
			gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
			break;
		}

			//For Sea units we want the list to only include Coastal cities
		case DOMAIN_SEA: {

			gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
			int buttonId = 0;
			foreach_(const CvCity* pCity, kPlayer.cities())
			{
				if (pCity->isCoastal(10))
				{
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText(pCity->getName()).c_str(), NULL, buttonId, WIDGET_CITY_GOTO, pCity->getID());
					buttonId++;
				}
			}

			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, buttonId, WIDGET_GENERAL);
			gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
			break;
		}
		}
	}

	return true;
}



bool CvDLLButtonPopup::launchChooseTechPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& player = GET_PLAYER(GC.getGame().getActivePlayer());

	int iDiscover = info.getData1();
	CvWString szHeader = info.getText();
	if (szHeader.empty())
	{
		szHeader = (iDiscover > 0) ? gDLL->getText("TXT_KEY_POPUP_CHOOSE_TECH").c_str() : gDLL->getText("TXT_KEY_POPUP_RESEARCH_NEXT").c_str();
	}
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, szHeader, DLL_FONT_LEFT_JUSTIFY);

	if (iDiscover == 0)
	{
		// Allow user to Jump to the Tech Chooser
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_SEE_BIG_PICTURE").c_str(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_TECH")->getPath(), GC.getNumTechInfos(), WIDGET_GENERAL, -1, MAX_INT, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
		// Note: This button is NOT supposed to close the popup!!
	}

	const TechTypes eBestTech = player.AI_bestTech(1, iDiscover > 0, true);
	const TechTypes eNextBestTech =
	(
		eBestTech == NO_TECH ? NO_TECH
		:
		player.AI_bestTech(1, iDiscover > 0, true, eBestTech, (AdvisorTypes)GC.getTechInfo(eBestTech).getAdvisorType())
	);

	int iNumTechs = 0;
	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if ((iI == eBestTech || iI == eNextBestTech) == (iPass == 0) && player.canResearch((TechTypes)iI))
			{
				CvWString szBuffer;
				szBuffer.Format(L"%s (%d)", GC.getTechInfo((TechTypes)iI).getDescription(), ((iDiscover > 0) ? 0 : player.getResearchTurnsLeft(((TechTypes)iI), true)));

				if ((iI == eBestTech) || (iI == eNextBestTech))
				{
					szBuffer += gDLL->getText("TXT_KEY_POPUP_RECOMMENDED_ONLY_ADV", GC.getAdvisorInfo((AdvisorTypes)(GC.getTechInfo((TechTypes)iI).getAdvisorType())).getTextKeyWide());
				}
				CvString szButton = GC.getTechInfo((TechTypes) iI).getButton();

				if (player.canFoundReligion() && GC.getGame().isTechCanFoundReligion((TechTypes)iI))
				{
					for (int iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
					{
						if (GC.getReligionInfo((ReligionTypes)iJ).getTechPrereq() == iI)
						{
							if (GC.getGame().countKnownTechNumTeams((TechTypes)iI) < 1)
							{
								szButton = GC.getReligionInfo((ReligionTypes) iJ).getTechButton();
							}
							else
							{
								szButton = GC.getReligionInfo((ReligionTypes) iJ).getGenericTechButton();
							}
							break;
						}
					}
				}
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, szButton, iI, WIDGET_RESEARCH, iI, iDiscover, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
				iNumTechs++;
			}
		}
	}
	if (0 == iNumTechs)
	{
		return false; // player cannot research anything, so don't show this popup after all
	}

	gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_TECHNOLOGY, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_TECH")->getPath());

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, ((iDiscover > 0) ? POPUPSTATE_QUEUED : POPUPSTATE_MINIMIZED));

	return true;
}

bool CvDLLButtonPopup::launchChangeCivicsPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CivicTypes* paeNewCivics = new CivicTypes[GC.getNumCivicOptionInfos()];
	if (NULL == paeNewCivics)
	{
		return (false);
	}

	const CivicTypes eCivicType = (CivicTypes)info.getData2();
	bool bValid = true;

	if (eCivicType != NO_CIVIC)
	{
		const CivicOptionTypes eCivicOption = (CivicOptionTypes)info.getData1();
		bValid = false;
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			if (iI == eCivicOption)
			{
				paeNewCivics[iI] = eCivicType;
			}
			else
			{
				paeNewCivics[iI] = GET_PLAYER(GC.getGame().getActivePlayer()).getCivics((CivicOptionTypes)iI);
			}
		}
		if (GET_PLAYER(GC.getGame().getActivePlayer()).canRevolution(paeNewCivics))
		{
			bValid = true;
		}
	}

	if (bValid)
	{
		CvWString szBuffer;
		if (eCivicType != NO_CIVIC)
		{
			szBuffer = gDLL->getText("TXT_KEY_POPUP_NEW_CIVIC", GC.getCivicInfo(eCivicType).getTextKeyWide());
			if (!CvWString(GC.getCivicInfo(eCivicType).getStrategy()).empty())
			{
				CvWString szTemp;
				szTemp.Format(L" (%s)", GC.getCivicInfo(eCivicType).getStrategy());
				szBuffer += szTemp;
			}
			szBuffer += gDLL->getText("TXT_KEY_POPUP_START_REVOLUTION");
			gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);

			szBuffer = gDLL->getText("TXT_KEY_POPUP_YES_START_REVOLUTION");
			const int iAnarchyLength = GET_PLAYER(GC.getGame().getActivePlayer()).getCivicAnarchyLength(paeNewCivics);
			if (iAnarchyLength > 0)
			{
				szBuffer += gDLL->getText("TXT_KEY_POPUP_TURNS_OF_ANARCHY", iAnarchyLength);
			}
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, NULL, 0, WIDGET_GENERAL);
		}
		else
		{
			gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_FIRST_REVOLUTION"));
		}
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_OLD_WAYS_BEST").c_str(), NULL, 1, WIDGET_GENERAL);
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_SEE_BIG_PICTURE").c_str(), NULL, 2, WIDGET_GENERAL);
		gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_CIVIC, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_CIVICS")->getPath());
		gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_MINIMIZED);
	}
	SAFE_DELETE_ARRAY(paeNewCivics);

	return bValid;
}


bool CvDLLButtonPopup::launchAlarmPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->playGeneralSound("AS2D_ALARM");
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_ALARM_TITLE").c_str());
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, info.getText());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchDeclareWarMovePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	const TeamTypes eRivalTeam = (TeamTypes)info.getData1();

	FAssert(eRivalTeam != NO_TEAM);

	const CvPlot* pPlot = GC.getMap().plot(info.getData2(), info.getData3());

	CvWString szBuffer;
	if (pPlot != NULL && pPlot->getTeam() == eRivalTeam)
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_ENTER_LANDS_WAR", GET_PLAYER(pPlot->getOwner()).getCivilizationAdjective());

		if (GET_TEAM(GC.getGame().getActiveTeam()).isOpenBordersTrading())
		{
			szBuffer += gDLL->getText("TXT_KEY_POPUP_ENTER_WITH_OPEN_BORDERS");
		}
	}
	else
	{
		szBuffer = gDLL->getText("TXT_KEY_POPUP_DOES_THIS_MEAN_WAR", GET_TEAM(eRivalTeam).getName().GetCString());
	}
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_DECLARE_WAR_YES").c_str(), NULL, 0);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_DECLARE_WAR_NO").c_str());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchConfirmCommandPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(
		pPopup,
		gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE_ACTION", GC.getActionInfo(info.getData1()).getTextKeyWide()).c_str()
	);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, 0);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}


bool CvDLLButtonPopup::launchLoadUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvSelectionGroup* pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();

	if (NULL == pSelectionGroup)
	{
		return (false);
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_TRANSPORT"));

	const CvPlot* pPlot = pSelectionGroup->plot();
	if (NULL == pPlot)
	{
		return (false);
	}

	int iCount = 1;
	CvUnit* pFirstUnit = NULL;
	foreach_(CvUnit* pLoopUnit, pPlot->units())
	{
		if (pSelectionGroup->canDoCommand(COMMAND_LOAD_UNIT, pLoopUnit->getOwner(), pLoopUnit->getID()))
		{
			if (!pFirstUnit)
			{
				pFirstUnit = pLoopUnit;
			}
			CvWStringBuffer szBuffer;
			GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
			szBuffer.append(L", ");
			if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
			{
				szBuffer.append(gDLL->getText("TXT_KEY_UNITHELP_CARGO_SPACE", pLoopUnit->SMgetCargo(), pLoopUnit->SMcargoSpace()));
			}
			else
			{
				szBuffer.append(gDLL->getText("TXT_KEY_UNITHELP_CARGO_SPACE", pLoopUnit->getCargo(), pLoopUnit->cargoSpace()));
			}
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), NULL, iCount, WIDGET_GENERAL);
			iCount++;
		}
	}

	if (iCount <= 2)
	{
		if (pFirstUnit)
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_DO_COMMAND, COMMAND_LOAD_UNIT, pFirstUnit->getOwner(), pFirstUnit->getID());
		}
		return (false);
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}


bool CvDLLButtonPopup::launchLeadUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvSelectionGroup*  pSelectionGroup = gDLL->getInterfaceIFace()->getSelectionList();
	if (NULL == pSelectionGroup)
	{
		return (false);
	}

	const CvPlot* pPlot = pSelectionGroup->plot();
	if (NULL == pPlot)
	{
		return (false);
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_UNIT_TO_LEAD"));

	int iCount = 1;
	CvUnit* pFirstUnit = NULL;
	foreach_(CvUnit* pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->canPromote((PromotionTypes) info.getData1(), info.getData2()))
		{
			if (!pFirstUnit)
			{
				pFirstUnit = pLoopUnit;
			}
			CvWStringBuffer szBuffer;
			GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), NULL, iCount, WIDGET_GENERAL);
			iCount++;
		}
	}

	if (iCount <= 2)
	{
		if (pFirstUnit)
		{
			GC.getGame().selectionListGameNetMessage(GAMEMESSAGE_PUSH_MISSION, MISSION_LEAD, pFirstUnit->getID());
		}
		return (false);
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchDoEspionagePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvUnit* pUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
	if (NULL == pUnit)
	{
		return (false);
	}

	CvPlot* pPlot = pUnit->plot();
	if (NULL == pPlot)
	{
		return (false);
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_ESPIONAGE_MISSION"));

	for (int iLoop = 0; iLoop < GC.getNumEspionageMissionInfos(); iLoop++)
	{
		if (!GC.getEspionageMissionInfo((EspionageMissionTypes) iLoop).isPassive())
		{
			if (GET_PLAYER(pUnit->getOwner()).canDoEspionageMission((EspionageMissionTypes) iLoop, pPlot->getOwner(), pPlot, -1, pUnit))
			{
				if (GC.getEspionageMissionInfo((EspionageMissionTypes) iLoop).isTwoPhases())
				{
					CvWString szBuffer = GC.getEspionageMissionInfo((EspionageMissionTypes) iLoop).getDescription();
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, ARTFILEMGR.getInterfaceArtInfo("ESPIONAGE_BUTTON")->getPath(), iLoop, WIDGET_GENERAL);
				}
				else
				{
					int iCost = GET_PLAYER(pUnit->getOwner()).getEspionageMissionCost((EspionageMissionTypes) iLoop, pPlot->getOwner(), pPlot, -1, pUnit);
					if (iCost > 0)
					{
						CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", GC.getEspionageMissionInfo((EspionageMissionTypes) iLoop).getTextKeyWide(), iCost);
						gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, ARTFILEMGR.getInterfaceArtInfo("ESPIONAGE_BUTTON")->getPath(), iLoop, WIDGET_HELP_ESPIONAGE_COST, iLoop, -1);
					}
				}
			}
		}
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), NO_ESPIONAGEMISSION, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchDoEspionageTargetPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvUnit* pUnit = gDLL->getInterfaceIFace()->getHeadSelectedUnit();
	if (NULL == pUnit)
	{
		return false;
	}

	CvPlot* pPlot = pUnit->plot();
	PlayerTypes eTargetPlayer = pPlot->getOwner();

	if (NO_PLAYER == eTargetPlayer)
	{
		return false;
	}

	EspionageMissionTypes eMission = (EspionageMissionTypes)info.getData1();
	if (NO_ESPIONAGEMISSION == eMission)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_ESPIONAGE_CHOOSE_TARGET"));

	const CvCity* pCity = pPlot->getPlotCity();
	const CvPlayer& kPlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	const CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);
	if (kMission.getDestroyBuildingCostFactor() > 0)
	{
		if (NULL != pCity)
		{
			for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
			{
				if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iBuilding, pUnit)
				&& pCity->getNumActiveBuilding((BuildingTypes)iBuilding) > 0)
				{
					const CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iBuilding);
					const int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iBuilding, pUnit);
					CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", kBuilding.getDescription(), iCost);
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, kBuilding.getButton(), iBuilding, WIDGET_HELP_ESPIONAGE_COST, eMission, iBuilding);
				}
			}
		}
	}
	else if (kMission.getDestroyUnitCostFactor() > 0)
	{
		// RevolutionDCM start
		if (NULL != pCity)
		{
			for (int iSpecialist = 0; iSpecialist < GC.getNumSpecialistInfos(); iSpecialist++)
			{
				if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iSpecialist, pUnit))
				{
					//does this city contain this great specialist type?
					if (pCity->getFreeSpecialistCount((SpecialistTypes)iSpecialist) > 0)
					{
						const CvSpecialistInfo& kSpecialist = GC.getSpecialistInfo((SpecialistTypes)iSpecialist);
						const int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iSpecialist, pUnit);
						CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", kSpecialist.getDescription(), iCost);
						gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, kSpecialist.getButton(), iSpecialist, WIDGET_HELP_ESPIONAGE_COST, eMission, iSpecialist);
					}
				}
			}
		}
		// RevolutionDCM end
	}
	else if (kMission.getDestroyProjectCostFactor() > 0)
	{
		for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iProject, pUnit))
			{
				if (GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getProjectCount((ProjectTypes)iProject) > 0)
				{
					int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iProject, pUnit);
					CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", GC.getProjectInfo((ProjectTypes)iProject).getDescription(), iCost);
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, GC.getProjectInfo((ProjectTypes)iProject).getButton(), iProject, WIDGET_HELP_ESPIONAGE_COST, eMission, iProject);
				}
			}
		}
	}
	else if (kMission.getBuyTechCostFactor() > 0)
	{
		for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iTech, pUnit))
			{
				const int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iTech, pUnit);
				const CvTechInfo& kTech = GC.getTechInfo((TechTypes)iTech);
				CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", kTech.getDescription(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, kTech.getButton(), iTech, WIDGET_HELP_ESPIONAGE_COST, eMission, iTech);
			}
		}
	}
	else if (kMission.getSwitchCivicCostFactor() > 0)
	{
		for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iCivic, pUnit))
			{
				const int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iCivic, pUnit);
				const CvCivicInfo& kCivic = GC.getCivicInfo((CivicTypes)iCivic);
				CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", kCivic.getDescription(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, kCivic.getButton(), iCivic, WIDGET_HELP_ESPIONAGE_COST, eMission, iCivic);
			}
		}
	}
	else if (kMission.getSwitchReligionCostFactor() > 0)
	{
		for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iReligion, pUnit))
			{
				const int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iReligion, pUnit);
				const CvReligionInfo& kReligion = GC.getReligionInfo((ReligionTypes)iReligion);
				CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", kReligion.getDescription(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, kReligion.getButton(), iReligion, WIDGET_HELP_ESPIONAGE_COST, eMission, iReligion);
			}
		}
	}
/************************************************************************************************/
/* Afforess	                  Start		 07/28/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	else if (kMission.getRemoveReligionsCostFactor() > 0)
	{
		for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iReligion, pUnit))
			{
				const int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iReligion, pUnit);
				const CvReligionInfo& kReligion = GC.getReligionInfo((ReligionTypes)iReligion);
				CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", kReligion.getDescription(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, kReligion.getButton(), iReligion, WIDGET_HELP_ESPIONAGE_COST, eMission, iReligion);
			}
		}
	}
	else if (kMission.getRemoveCorporationsCostFactor() > 0)
	{
		for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); ++iCorp)
		{
			if (kPlayer.canDoEspionageMission(eMission, eTargetPlayer, pPlot, iCorp, pUnit))
			{
				const int iCost = kPlayer.getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iCorp, pUnit);
				const CvCorporationInfo& kCorp = GC.getCorporationInfo((CorporationTypes)iCorp);
				CvWString szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_COST", kCorp.getDescription(), iCost);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, kCorp.getButton(), iCorp, WIDGET_HELP_ESPIONAGE_COST, eMission, iCorp);
			}
		}
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), NO_ESPIONAGEMISSION, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}


bool CvDLLButtonPopup::launchMainMenuPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetStyle( pPopup, "Window_NoTitleBar_Style" );

	// 288,72
	gDLL->getInterfaceIFace()->popupAddDDS(pPopup, "resource/temp/civ4_title_small.dds", 192, 48);

	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_DESKTOP").c_str(), NULL, 0, WIDGET_GENERAL, 0, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);

	if (!gDLL->getInterfaceIFace()->isDebugMenuCreated())
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXIT_TO_MAIN_MENU").c_str(), NULL, 1, WIDGET_GENERAL, 1, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGame().canDoControl(CONTROL_RETIRE))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_RETIRE").c_str(), NULL, 2, WIDGET_GENERAL, 2, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if ((GC.getGame().getElapsedGameTurns() == 0) && !(GC.getGame().isGameMultiPlayer()) && !(GC.getInitCore().getWBMapScript()))
	{
		bool bShow = true;
		for (int i = 0; i < GC.getMap().numPlots(); ++i)
		{
			CvPlot* pPlot = GC.getMap().plotByIndex(i);
			if (!pPlot->getScriptData().empty())
			{
				bShow = false;
				break;
			}
		}
		if (bShow)
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_REGENERATE_MAP").c_str(), NULL, 3, WIDGET_GENERAL, 3, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
		}
	}

	if (GC.getGame().canDoControl(CONTROL_LOAD_GAME))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_LOAD_GAME").c_str(), NULL, 4, WIDGET_GENERAL, 4, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}
	if (GC.getGame().canDoControl(CONTROL_SAVE_NORMAL))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_SAVE_GAME").c_str(), NULL, 5, WIDGET_GENERAL, 5, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_OPTIONS").c_str(), NULL, 6, WIDGET_GENERAL, 6, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);

	if (GC.getGame().canDoControl(CONTROL_WORLD_BUILDER))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_ENTER_WB").c_str(), NULL, 7, WIDGET_GENERAL, 7, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGame().canDoControl(CONTROL_ADMIN_DETAILS))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_GAME_DETAILS").c_str(), NULL, 8, WIDGET_GENERAL, 8, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	if (GC.getGame().canDoControl(CONTROL_DETAILS))
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_TITLE").c_str(), NULL, 9, WIDGET_GENERAL, 9, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL").c_str(), NULL, 10, WIDGET_GENERAL, 10, 0, true, POPUP_LAYOUT_STRETCH, DLL_FONT_CENTER_JUSTIFY);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchConfirmMenu(CvPopup *pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ARE_YOU_SURE").c_str());
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES").c_str(), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO").c_str(), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchPythonScreen(CvPopup* pPopup, CvPopupInfo &info)
{
	// this is not really a popup, but a Python screen
	// we trick the app into thinking that it's a popup so that we can take advantage of the popup queuing system
	Cy::call(PYScreensModule, CvString(info.getText()).GetCString(), Cy::Args()
		<< info.getData1()
		<< info.getData2()
		<< info.getData3()
		<< info.getOption1()
		<< info.getOption2()
	);

	return false; // return false, so the Popup object is deleted, since it's just a dummy
}

bool CvDLLButtonPopup::launchCancelDeal(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup,  gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL") );

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL_YES"), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_CANCEL_DEAL_NO"), NULL, 1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchPythonPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, info.getText());
	for (int i = 0; i < info.getNumPythonButtons(); i++)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, info.getPythonButtonText(i), info.getPythonButtonArt(i).IsEmpty() ? NULL : info.getPythonButtonArt(i).GetCString(), i);
	}

	gDLL->getInterfaceIFace()->popupSetPopupType(pPopup, POPUPEVENT_WARNING, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_POPUPBUTTON_WARNING")->getPath());
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchDetailsPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	if (!info.getOption1())
	{
		gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_TITLE"));

		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_LEADER_NAME"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGame().getActivePlayer()).getName(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_LEADER_NAME"), 0, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_CIV_DESC"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGame().getActivePlayer()).getCivilizationDescription(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_DESC"), 1, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_CIV_SHORT_DESC"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGame().getActivePlayer()).getCivilizationShortDescription(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_SHORT_DESC"), 2, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MENU_CIV_ADJ"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GET_PLAYER(GC.getGame().getActivePlayer()).getCivilizationAdjective(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MENU_CIV_ADJ"), 3, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYERINFO_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	}
	else if (!GC.getInitCore().getCivPassword(GC.getInitCore().getActivePlayer()).empty())
	{
		// the purpose of the popup with the option set to true is to ask for the civ password if it's not set
		return false;
	}
	if (GC.getGame().isPbem() || GC.getGame().isHotSeat() || GC.getGame().isPitboss())
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MAIN_MENU_PASSWORD"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, PASSWORD_DEFAULT, WIDGET_GENERAL, gDLL->getText("TXT_KEY_MAIN_MENU_PASSWORD"), 4, POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	}
	if ( (GC.getGame().isPitboss() || GC.getGame().isPbem()) && !info.getOption1() )
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_EMAIL"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, CvWString(GC.getInitCore().getEmail(GC.getInitCore().getActivePlayer())), WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_DETAILS_EMAIL"), 5, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYEREMAIL_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	}
	if (GC.getGame().isPbem() && !info.getOption1())
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_DETAILS_SMTP"));
		gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, CvWString(GC.getInitCore().getSmtpHost(GC.getInitCore().getActivePlayer())), WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_DETAILS_SMTP"), 6, POPUP_LAYOUT_STRETCH, 0, MAX_PLAYEREMAIL_CHAR_COUNT);
		gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);

		if (GC.getGame().getPbemTurnSent())
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_MISC_SEND"), NULL, 0, WIDGET_GENERAL);
		}
	}

	// Disable leader name edit box for internet games
	if (GC.getInitCore().getMultiplayer() && gDLL->isFMPMgrPublic())
	{
		gDLL->getInterfaceIFace()->popupEnableEditBox(pPopup, 0, false);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}

bool CvDLLButtonPopup::launchAdminPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_GAME_DETAILS"));

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_MAIN_MENU_GAME_NAME"));
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, GC.getInitCore().getGameName(), WIDGET_GENERAL, gDLL->getText("TXT_KEY_MAIN_MENU_GAME_NAME"), 0, POPUP_LAYOUT_STRETCH, 0, MAX_GAMENAME_CHAR_COUNT);
	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"));
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, PASSWORD_DEFAULT, WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"), 1, POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);
	gDLL->getInterfaceIFace()->popupAddSeparator(pPopup);
	if (!GC.getGame().isGameMultiPlayer())
	{
		gDLL->getInterfaceIFace()->popupCreateCheckBoxes(pPopup, 1, 2);
		gDLL->getInterfaceIFace()->popupSetCheckBoxText(pPopup, 0, gDLL->getText("TXT_KEY_POPUP_ADMIN_ALLOW_CHEATS"), 2);
		gDLL->getInterfaceIFace()->popupSetCheckBoxState(pPopup, 0, gDLL->getChtLvl() > 0, 2);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchAdminPasswordPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"));
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup, L"", WIDGET_GENERAL, gDLL->getText("TXT_KEY_POPUP_ADMIN_PASSWORD"), 0, POPUP_LAYOUT_STRETCH, 0, MAX_PASSWORD_CHAR_COUNT);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchExtendedGamePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_EXTENDED_GAME_TITLE"));

	if (GC.getGame().countHumanPlayersAlive() > 0)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_EXTENDED_GAME_YES"), NULL, 0, WIDGET_GENERAL);
	}

	if (!gDLL->getInterfaceIFace()->isDebugMenuCreated())
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_EXTENDED_GAME_NO_MENU"), NULL, 1, WIDGET_GENERAL);
	}
	else
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_EXTENDED_GAME_NO_DESKTOP"), NULL, 1, WIDGET_GENERAL);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false);
	return (true);
}

bool CvDLLButtonPopup::launchDiplomacyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iCount;
	int iI;

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_DIPLOMACY_TITLE"));

	iCount = 0;

	for (iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER(GC.getGame().getActivePlayer()).canContact((PlayerTypes)iI))
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GET_PLAYER((PlayerTypes)iI).getName(), NULL, iI, WIDGET_GENERAL);
				iCount++;
			}
		}
	}

	if (iCount == 0)
	{
		return (false);
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, MAX_PC_PLAYERS, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return (true);
}


bool CvDLLButtonPopup::launchAddBuddyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_SYSTEM_ADD_BUDDY", info.getText().GetCString()) );
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false);
	return (true);
}

bool CvDLLButtonPopup::launchForcedDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_MAIN_MENU_FORCED_DISCONNECT_INGAME") );
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true);
	return (true);
}

bool CvDLLButtonPopup::launchPitbossDisconnectPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_PITBOSS_DISCONNECT") );
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true);
	return (true);
}

bool CvDLLButtonPopup::launchKickedPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString( pPopup, gDLL->getText("TXT_KEY_POPUP_KICKED") );
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true);
	return (true);
}

bool CvDLLButtonPopup::launchVassalDemandTributePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	if (info.getData1() == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kVassal = GET_PLAYER((PlayerTypes)info.getData1());
	if (!GET_TEAM(kVassal.getTeam()).isVassal(GC.getGame().getActiveTeam()))
	{
		return false;
	}

	int iNumResources = 0;
	if (kVassal.canTradeNetworkWith(GC.getGame().getActivePlayer()))
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_VASSAL_DEMAND_TRIBUTE", kVassal.getNameKey()));

		for (int iBonus = 0; iBonus < GC.getNumBonusInfos(); iBonus++)
		{
			if (kVassal.getNumTradeableBonuses((BonusTypes)iBonus) > 0 && GET_PLAYER(GC.getGame().getActivePlayer()).getNumAvailableBonuses((BonusTypes)iBonus) == 0)
			{
				const CvBonusInfo& info = GC.getBonusInfo((BonusTypes)iBonus);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, info.getDescription(), info.getButton(), iBonus, WIDGET_GENERAL, iBonus, -1, true, POPUP_LAYOUT_STRETCH, DLL_FONT_LEFT_JUSTIFY);
				++iNumResources;
			}
		}
	}

	if (iNumResources > 0)
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), GC.getNumBonusInfos(), WIDGET_GENERAL);
	}
	else
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_VASSAL_TRIBUTE_NOT_POSSIBLE"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), GC.getNumBonusInfos(), WIDGET_GENERAL);
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchVassalGrantTributePopup(CvPopup* pPopup, CvPopupInfo &info)
{
	if (info.getData1() == NO_PLAYER)
	{
		return false;
	}

	CvPlayer& kMaster = GET_PLAYER((PlayerTypes)info.getData1());
	if (!GET_TEAM(GC.getGame().getActiveTeam()).isVassal(kMaster.getTeam()))
	{
		return false;
	}

	if (info.getData2() == NO_BONUS)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE", kMaster.getCivilizationDescriptionKey(), kMaster.getNameKey(), GC.getBonusInfo((BonusTypes)info.getData2()).getTextKeyWide()));
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_YES"), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_VASSAL_GRANT_TRIBUTE_NO"), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchEventPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvPlayer& kActivePlayer = GET_PLAYER(GC.getGame().getActivePlayer());
	EventTriggeredData* pTriggeredData = kActivePlayer.getEventTriggered(info.getData1());
	if (NULL == pTriggeredData)
	{
		return false;
	}

	if (pTriggeredData->m_eTrigger == NO_EVENTTRIGGER)
	{
		return false;
	}

	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(pTriggeredData->m_eTrigger);
/************************************************************************************************/
/* Afforess	                  Start		 05/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
// Begin EmperorFool: Events with Images
	if (GET_PLAYER(GC.getGame().getActivePlayer()).isModderOption(MODDEROPTION_EVENT_IMAGES))
	{
		if (kTrigger.getEventArt())
		{
			gDLL->getInterfaceIFace()->popupAddDDS(pPopup, kTrigger.getEventArt());
		}
	}
// End EmperorFool: Events with Images
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/


	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, pTriggeredData->m_szText);

	bool bEventAvailable = false;
	for (int i = 0; i < kTrigger.getNumEvents(); i++)
	{
		if (GET_PLAYER(GC.getGame().getActivePlayer()).canDoEvent((EventTypes)kTrigger.getEvent(i), *pTriggeredData))
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getEventInfo((EventTypes)kTrigger.getEvent(i)).getDescription(), GC.getEventInfo((EventTypes)kTrigger.getEvent(i)).getButton(), kTrigger.getEvent(i), WIDGET_CHOOSE_EVENT, kTrigger.getEvent(i), info.getData1());
			bEventAvailable = true;
		}
		else
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, GC.getEventInfo((EventTypes)kTrigger.getEvent(i)).getDescription(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_EVENT_UNAVAILABLE_BULLET")->getPath(), -1, WIDGET_CHOOSE_EVENT, kTrigger.getEvent(i), info.getData1(), false);
		}
	}

	if (!bEventAvailable)
	{
		return false;
	}

	if (kTrigger.isPickCity())
	{
		const CvCity* pCity = kActivePlayer.getCity(pTriggeredData->m_iCityId);
		FAssert(NULL != pCity);
		if (NULL != pCity)
		{
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_EXAMINE_CITY").c_str(), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), GC.getNumEventInfos(), WIDGET_GENERAL, -1, -1);
		}
	}

	if (kTrigger.isShowPlot())
	{
		const CvPlot* pPlot = GC.getMap().plot(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
		if (NULL != pPlot && pPlot->isInViewport())
		{
			gDLL->getEngineIFace()->addColoredPlot(pPlot->getViewportX(), pPlot->getViewportY(), GC.getColorInfo(GC.getCOLOR_WARNING_TEXT()).getColor(), PLOT_STYLE_CIRCLE, PLOT_LANDSCAPE_LAYER_RECOMMENDED_PLOTS);
			gDLL->getInterfaceIFace()->lookAt(pPlot->getPoint(), CAMERALOOKAT_NORMAL);
		}
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, !bEventAvailable, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchFreeColonyPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_FREE_COLONY"));

	if (GET_PLAYER(ePlayer).canSplitEmpire())
	{
		foreach_(const CvArea* pLoopArea, GC.getMap().areas())
		{
			if (GET_PLAYER(ePlayer).canSplitArea(pLoopArea->getID()))
			{
				CvWString szCityList;
				int iNumCities = 0;
				foreach_(const CvCity* pLoopCity, GET_PLAYER(ePlayer).cities())
				{
					if (pLoopCity->area()->getID() == pLoopArea->getID())
					{
						if (!szCityList.empty())
						{
							szCityList += L", ";
						}
						++iNumCities;

						szCityList += pLoopCity->getName();
					}
				}

				const CvWString szBuffer = gDLL->getText("TXT_KEY_SPLIT_EMPIRE", szCityList.GetCString());
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), pLoopArea->getID(), WIDGET_GENERAL);
			}
		}
	}

	foreach_(const CvCity* pLoopCity, GET_PLAYER(ePlayer).cities())
	{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       08/04/09                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* original bts code
		PlayerTypes ePlayer = pLoopCity->getLiberationPlayer(false);
		if (NO_PLAYER != ePlayer)
		{
			CvWString szCity = gDLL->getText("TXT_KEY_CITY_LIBERATE", pLoopCity->getNameKey(), GET_PLAYER(ePlayer).getNameKey());
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szCity, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), -pLoopCity->getID(), WIDGET_GENERAL);
		}
*/
		// Avoid potential variable name conflict
		PlayerTypes eLibPlayer = pLoopCity->getLiberationPlayer(false);
		if (NO_PLAYER != eLibPlayer)
		{
			// Don't offer liberation during war
			if( !(GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(GET_PLAYER(eLibPlayer).getTeam())) )
			{
				const CvWString szCity = gDLL->getText("TXT_KEY_CITY_LIBERATE", pLoopCity->getNameKey(), GET_PLAYER(eLibPlayer).getNameKey());
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szCity, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CITYSELECTION")->getPath(), -pLoopCity->getID(), WIDGET_GENERAL);
			}
		}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), ARTFILEMGR.getInterfaceArtInfo("INTERFACE_BUTTONS_CANCEL")->getPath(), 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchLaunchPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	VictoryTypes eVictory = (VictoryTypes)info.getData1();
	if (NO_VICTORY == eVictory)
	{
		return false;
	}

	CvTeam& kTeam = GET_TEAM(GET_PLAYER(ePlayer).getTeam());

	if (kTeam.getVictoryCountdown(eVictory) > 0 || GC.getGame().getGameState() != GAMESTATE_ON)
	{
		return false;
	}

	CvWString szDate;
	GAMETEXT.setTimeStr(szDate, GC.getGame().getGameTurn() + kTeam.getVictoryDelay(eVictory), false);

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, GC.getVictoryInfo(eVictory).getCivilopedia());
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_ESTIMATED_VICTORY_DATE", szDate.GetCString()));

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchGetSaveFormatPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvWString szBuffer = gDLL->getText("TXT_KEY_POPUP_CHOOSE_SAVE_GAME_FORMAT");

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, szBuffer);

	szBuffer = gDLL->getText("TXT_KEY_POPUP_NEW_GAME_SAVE_FORMAT_BODY");
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_OLD_GAME_SAVE_FORMAT").c_str(), NULL, 0, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NEW_GAME_SAVE_FORMAT").c_str(), NULL, 1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}

bool CvDLLButtonPopup::launchModifierRecalculationPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	CvWString szBuffer = gDLL->getText("TXT_KEY_POPUP_MODIFIER_RECALCULATION");

	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, szBuffer);

	szBuffer = gDLL->getText("TXT_KEY_POPUP_MODIFIER_RECALCULATION_BODY");
	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, szBuffer);

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
	return true;
}

bool CvDLLButtonPopup::launchNameListPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, "List Name");
	gDLL->getInterfaceIFace()->popupCreateEditBox(pPopup);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, true, POPUPSTATE_IMMEDIATE);
	return true;
}

bool CvDLLButtonPopup::launchFoundReligionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}
/************************************************************************************************/
/* Afforess	                  Start		 06/17/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (!GET_PLAYER(ePlayer).m_bChoosingReligion || GET_PLAYER(ePlayer).getNumCities() == 0)
	{
		//AddDLLMessage(GC.getGame().getActivePlayer(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_CHEATERS_NEVER_PROSPER"), NULL, MESSAGE_TYPE_MAJOR_EVENT);
		return true;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_FOUNDED_RELIGION"));

	bool bFound = false;
	for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
	{
		if (!GC.getGame().isReligionFounded((ReligionTypes)iReligion))
		{
			const CvReligionInfo& kReligion = GC.getReligionInfo((ReligionTypes)iReligion);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kReligion.getDescription(), kReligion.getButton(), iReligion, WIDGET_GENERAL);
			bFound = true;
		}
	}

	if (!bFound)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}


bool CvDLLButtonPopup::invasionPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	const CvCity* pCity = GET_PLAYER(GC.getGame().getActivePlayer()).getCity(info.getData1());

	if ( pCity != NULL )
	{
		BuildingTypes eBuilding = NO_BUILDING;

		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			if (pCity->getNumRealBuilding((BuildingTypes)iI) > 0 && GC.getBuildingInfo((BuildingTypes)iI).getInvasionChance() > 0)
			{
				eBuilding = (BuildingTypes)iI;
				break;
			}
		}

		if (eBuilding == NO_BUILDING)
		{
			FErrorMsg("No Invasion Building Found! Error in CvDLLButtonPopup::invasionPopup(...)!");
			return false;
		}

		gDLL->getInterfaceIFace()->popupSetHeaderString(pPopup, gDLL->getText("TXT_KEY_POPUP_INVASION", pCity->getNameKey()), DLL_FONT_LEFT_JUSTIFY);

		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_POPUP_INVADED_CITY", GC.getBuildingInfo(eBuilding).getDescription(), pCity->getNameKey()));
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_THATS_TERRIBLE"), NULL, 0, WIDGET_GENERAL);
		gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);
		gDLL->getInterfaceIFace()->playGeneralSound("AS2D_BUILD_BARRACKS");
	}

	return true;
}

bool CvDLLButtonPopup::launchSelectShadowUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iUnitID = info.getData1();
	int iX = info.getData2();
	int iY = info.getData3();
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iUnitID);
	if (pUnit == NULL)
	{
		return false;
	}

	const CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_UNIT_TO_SHADOW"));

	foreach_(CvUnit* pLoopUnit, pPlot->units())
	{
		if (pUnit->canShadowAt(pPlot, pLoopUnit) && pLoopUnit->getID() != 0)
		{
			CvWStringBuffer szBuffer;
			GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), GC.getUnitInfo(pLoopUnit->getUnitType()).getButton(), pLoopUnit->getID(), WIDGET_GENERAL);
		}
	}

	//pPopup->setData1(iUnitID);
	//pPopup->setData2(iX);
	//pPopup->setData3(iY);

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchSelectDiscoveryTechPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iUnitID = info.getData1();
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iUnitID);
	if (pUnit == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_TECH_TO_WAIT_FOR"));

	CvWString szBuffer;
	//Find the last tech
	int iBestX = 0;
	for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
	{
		if (GET_PLAYER(ePlayer).canEverResearch((TechTypes)iJ))
		{
			if (GC.getTechInfo((TechTypes)iJ).getGridX() > iBestX)
			{
				iBestX = GC.getTechInfo((TechTypes)iJ).getGridX();
			}
		}
	}
	//Forces sorting by the columns in the tech screen
	for (int iColumn = 0; iColumn < iBestX + 1; iColumn++)
	{
		TechTypes eBestTech = NO_TECH;
		int iBestValue = 0;
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GC.getTechInfo((TechTypes)iI).getGridX() == iColumn)
			{
				if (GET_PLAYER(ePlayer).canEverResearch((TechTypes)iI) && !GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isHasTech((TechTypes)iI))
				{
					int iValue = 0;

					for (int iJ = 0; iJ < GC.getNumFlavorTypes(); iJ++)
					{
						iValue += (GC.getTechInfo((TechTypes) iI).getFlavorValue(iJ) * GC.getUnitInfo(pUnit->getUnitType()).getFlavorValue(iJ));
					}
					if (iValue > iBestValue)
					{
						eBestTech = (TechTypes)iI;
						iBestValue = iValue;
					}
				}
			}
		}
		if (eBestTech != NO_TECH)
		{
			szBuffer.Format(L"%s", GC.getTechInfo(eBestTech).getDescription());
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, szBuffer, GC.getTechInfo(eBestTech).getButton(), int(eBestTech), WIDGET_GENERAL);
		}
	}



	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, GC.getNumTechInfos(), WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchChooseBuildUpPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(info.getData1());
	if (pUnit == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_BUILDUP_CHOOSE", pUnit->getNameKey()));

	bool bSelected = false;
	for (std::map<PromotionLineTypes, PromotionLineKeyedInfo>::const_iterator it = pUnit->getPromotionLineKeyedInfo().begin(), end = pUnit->getPromotionLineKeyedInfo().end(); it != end; ++it)
	{
		if (it->second.m_bValidBuildUp)
		{
			const PromotionLineTypes ePromotionLine = it->first;
			const CvPromotionLineInfo& kPromotionLine = GC.getPromotionLineInfo(ePromotionLine);
			gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kPromotionLine.getDescription(), kPromotionLine.getButton(), ePromotionLine, WIDGET_HELP_BUILDUP, ePromotionLine);
			bSelected = true;
		}
	}

	if (!bSelected)
	{
		pUnit->establishBuildups();
		for (std::map<PromotionLineTypes, PromotionLineKeyedInfo>::const_iterator it = pUnit->getPromotionLineKeyedInfo().begin(), end = pUnit->getPromotionLineKeyedInfo().end(); it != end; ++it)
		{
			if (it->second.m_bValidBuildUp)
			{
				const PromotionLineTypes ePromotionLine = it->first;
				const CvPromotionLineInfo& kPromotionLine = GC.getPromotionLineInfo(ePromotionLine);
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kPromotionLine.getDescription(), kPromotionLine.getButton(), ePromotionLine, WIDGET_HELP_BUILDUP, ePromotionLine);
				bSelected = true;
			}
		}
	}

	if(!bSelected)
	{
		return false;
	}

	if (pUnit->isBuildUp())
	{
		gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_BUILDUP_DISBAND"), NULL, 0, WIDGET_GENERAL);
	}
	else gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_QUEUED);

	return true;
}

bool CvDLLButtonPopup::launchChooseTraitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	const PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_LEADER_LEVELUP"));

	bool bSelected = false;
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iTrait);
		const TraitTypes eTrait = ((TraitTypes)iTrait);
		if (!GET_PLAYER(ePlayer).hasTrait(eTrait) && !GC.getTraitInfo(eTrait).isNegativeTrait() && !GC.getTraitInfo(eTrait).isCivilizationTrait())
		{
			if (GET_PLAYER(ePlayer).canLearnTrait(eTrait))
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kTrait.getDescription(), kTrait.getButton(), iTrait, WIDGET_HELP_TRAIT, iTrait);
				bSelected = true;
			}
		}
		else if (GET_PLAYER(ePlayer).hasTrait(eTrait) && GC.getTraitInfo(eTrait).isNegativeTrait() && !GC.getTraitInfo(eTrait).isCivilizationTrait())
		{
			if (GET_PLAYER(ePlayer).canUnlearnTrait(eTrait, true))
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kTrait.getDescription(), kTrait.getButton(), iTrait, WIDGET_HELP_TRAIT, iTrait);
				bSelected = true;
			}
		}
	}

	if (!bSelected)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchChooseTraitPopupNegative(CvPopup* pPopup, CvPopupInfo &info)
{
	const PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_LEADER_LEVELUP_NEGATIVE"));

	bool bSelected = false;
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); ++iTrait)
	{
		const CvTraitInfo& kTrait = GC.getTraitInfo((TraitTypes)iTrait);
		const TraitTypes eTrait = ((TraitTypes)iTrait);
		if (!GET_PLAYER(ePlayer).hasTrait(eTrait) && GC.getTraitInfo(eTrait).isNegativeTrait() && !GC.getTraitInfo(eTrait).isCivilizationTrait())
		{
			if (GET_PLAYER(ePlayer).canLearnTrait(eTrait, true))
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kTrait.getDescription(), kTrait.getButton(), iTrait, WIDGET_HELP_TRAIT, iTrait);
				bSelected = true;
			}
		}
		else if (GET_PLAYER(ePlayer).hasTrait(eTrait) && !GC.getTraitInfo(eTrait).isNegativeTrait() && !GC.getTraitInfo(eTrait).isCivilizationTrait())
		{
			if (GET_PLAYER(ePlayer).canUnlearnTrait(eTrait, false))
			{
				gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kTrait.getDescription(), kTrait.getButton(), iTrait, WIDGET_HELP_TRAIT, iTrait);
				bSelected = true;
			}
		}
	}

	if (!bSelected)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return true;
}

bool CvDLLButtonPopup::launchSelectMergeUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iUnitID = info.getData1();
	int iX = info.getData2();
	int iY = info.getData3();
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iUnitID);
	if (pUnit == NULL)
	{
		return false;
	}

	const CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_UNIT_TO_MERGE"));

	foreach_(const CvUnit* pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->getOwner() == pUnit->getOwner())
		{
			if (GET_PLAYER(pLoopUnit->getOwner()).getBaseMergeSelectionUnit() != pLoopUnit->getID() && GET_PLAYER(pLoopUnit->getOwner()).getFirstMergeSelectionUnit() != pLoopUnit->getID())
			{
				if (pLoopUnit->getUnitType() == pUnit->getUnitType() && pLoopUnit->groupRank() == pUnit->groupRank() && pLoopUnit->qualityRank() == pUnit->qualityRank())
				{
					if (!pLoopUnit->isHurt() && !pLoopUnit->isDead() && !pLoopUnit->isFighting() && !pLoopUnit->isCargo() && !pLoopUnit->hasCargo() && !pLoopUnit->isDelayedDeath() && !pLoopUnit->isSpy() && !pLoopUnit->hasMoved() && (pLoopUnit->baseWorkRate() < 1) && (pLoopUnit->groupRank() < pLoopUnit->eraGroupMergeLimit()))
					{
						if (!pLoopUnit->hasCannotMergeSplit())
						{
							CvWStringBuffer szBuffer;
							GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
							gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), GC.getUnitInfo(pLoopUnit->getUnitType()).getButton(), pLoopUnit->getID(), WIDGET_GENERAL);
						}
					}
				}
			}
		}
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, -1, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_QUEUED);

	return (true);
}

bool CvDLLButtonPopup::launchConfirmSplitUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	// int iUnitID = info.getData1(); Unused
	int iX = info.getData2();
	int iY = info.getData3();
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CONFIRM_UNIT_SPLIT"));

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_YES"), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}

bool CvDLLButtonPopup::launchImprovementUpgradeOptionsPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	if (GC.getGame().getActivePlayer() == NO_PLAYER)
	{
		return false;
	}
	int iX = info.getData2();
	int iY = info.getData3();

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
	{
		return false;
	}
	GET_PLAYER(GC.getGame().getActivePlayer()).setFocusPlots(iX, iY);

	if (GC.getGame().getActivePlayer() == pPlot->getOwner())
	{
		gDLL->getInterfaceIFace()->lookAt(pPlot->getPoint(), CAMERALOOKAT_IMMEDIATE);
		EffectTypes eEffect = (EffectTypes)GC.getInfoTypeForString("EFFECT_CREATION");
		gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
		gDLL->getInterfaceIFace()->playGeneralSound("AS2D_UNIT_BUILD_WORKER", pPlot->getPoint());
	}
	const int iCurrentImprovement = info.getData1();
	const CvImprovementInfo& kImprovement = GC.getImprovementInfo((ImprovementTypes)iCurrentImprovement);
	const TeamTypes eTeam = GET_PLAYER(GC.getGame().getActivePlayer()).getTeam();

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_IMPROVEMENTHELP_UPGRADE_OPTIONS"));

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		if (iI == iCurrentImprovement) continue;

		const ImprovementTypes eImprovement = (ImprovementTypes)iI;
		const CvImprovementInfo& kImprovementX = GC.getImprovementInfo(eImprovement);

		// Toffer - Upgrade cost code commented out in setImprovementType() for the time being
		//if (kImprovementX.getHighestCost() <= GET_PLAYER(pPlot->getOwner()).getGold())
		{
			if (kImprovement.getImprovementUpgrade() == eImprovement)
			{
				if (pPlot->canHaveImprovement(eImprovement, eTeam, false, true))
				{
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kImprovementX.getDescription(), kImprovementX.getButton(), iI, WIDGET_HELP_IMPROVEMENT_ACTUAL, iI, 0, true);
				}
				continue;
			}
			for (int iJ = 0; iJ < kImprovement.getNumAlternativeImprovementUpgradeTypes(); iJ++)
			{
				if ((ImprovementTypes)kImprovement.getAlternativeImprovementUpgradeType(iJ) == eImprovement
				&& pPlot->canHaveImprovement(eImprovement, eTeam, false, true))
				{
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, kImprovementX.getDescription(), kImprovementX.getButton(), iI, WIDGET_HELP_IMPROVEMENT_ACTUAL, iI, 0, true);
				}
			}
		}
	}
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_IMPROVEMENTHELP_UPGRADE_NO"), NULL, GC.getNumImprovementInfos(), WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false);

	return true;
}

bool CvDLLButtonPopup::launchSelectArrestUnitPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iUnitID = info.getData1();
	int iX = info.getData2();
	int iY = info.getData3();
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	CvUnit* pUnit = GET_PLAYER(ePlayer).getUnit(iUnitID);
	if (pUnit == NULL)
	{
		return false;
	}

	const CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
	{
		return false;
	}

	gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CHOOSE_UNIT_TO_ARREST"));

	foreach_(const CvUnit* pLoopUnit, pPlot->units())
	{
		if (pLoopUnit->isWanted())
		{
			if (GET_PLAYER(pLoopUnit->getOwner()).getArrestingUnit() != pLoopUnit->getID())
			{
				if (!pLoopUnit->isInvisible(GET_PLAYER(ePlayer).getTeam(), false) && !pLoopUnit->isDead() && !pLoopUnit->isFighting() && !pLoopUnit->isDelayedDeath() && !pLoopUnit->isSpy())
				{
					CvWStringBuffer szBuffer;
					GAMETEXT.setUnitHelp(szBuffer, pLoopUnit, true);
					gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), GC.getUnitInfo(pLoopUnit->getUnitType()).getButton(), pLoopUnit->getID(), WIDGET_GENERAL);
				}
			}
		}
	}

	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_NEVER_MIND"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_QUEUED);

	return (true);
}

bool CvDLLButtonPopup::launchConfirmAmbushPopup(CvPopup* pPopup, CvPopupInfo &info)
{
	int iUnitID = info.getData1();
	int iX = info.getData2();
	int iY = info.getData3();
	PlayerTypes ePlayer = GC.getGame().getActivePlayer();
	CvWStringBuffer szBuffer;
	if (ePlayer == NO_PLAYER)
	{
		return false;
	}

	if (iUnitID != GET_PLAYER(ePlayer).getAmbushingUnit())
	{
		return false;
	}

	bool bAssassinate = GET_PLAYER(ePlayer).isAssassinate();

	CvPlot* pPlot = GC.getMap().plot(iX, iY);
	if (pPlot == NULL)
	{
		return false;
	}

	GAMETEXT.setCombatPlotHelp(szBuffer, pPlot, bAssassinate);
	if (bAssassinate)
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CONFIRM_ASSASSINATE"));
	}
	else
	{
		gDLL->getInterfaceIFace()->popupSetBodyString(pPopup, gDLL->getText("TXT_KEY_CONFIRM_AMBUSH"));
	}


	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, CvWString(szBuffer.getCString()), NULL, 1, WIDGET_GENERAL);
	gDLL->getInterfaceIFace()->popupAddGenericButton(pPopup, gDLL->getText("TXT_KEY_POPUP_NO"), NULL, 0, WIDGET_GENERAL);

	gDLL->getInterfaceIFace()->popupLaunch(pPopup, false, POPUPSTATE_IMMEDIATE);

	return (true);
}