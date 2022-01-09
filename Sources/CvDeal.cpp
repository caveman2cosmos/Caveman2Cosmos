// CvDeal.cpp

#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvDeal.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvTeamAI.h"

// Public Functions...

CvDeal::CvDeal()
{
	reset();
}


CvDeal::~CvDeal()
{
	uninit();
}


void CvDeal::init(int iID, PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer)
{
	//--------------------------------
	// Init saved data
	reset(iID, eFirstPlayer, eSecondPlayer);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
	setInitialGameTurn(GC.getGame().getGameTurn());
}


void CvDeal::uninit()
{
	m_firstTrades.clear();
	m_secondTrades.clear();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvDeal::reset(int iID, PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer)
{
	//--------------------------------
	// Uninit class
	uninit();

	m_iID = iID;
	m_iInitialGameTurn = 0;

	m_eFirstPlayer = eFirstPlayer;
	m_eSecondPlayer = eSecondPlayer;
}


void CvDeal::kill(bool bKillTeam)
{
	if (getLengthFirstTrades() > 0 || getLengthSecondTrades() > 0)
	{
		CvWString szString;
		CvWStringBuffer szDealString;
		CvWString szCancelString = gDLL->getText("TXT_KEY_POPUP_DEAL_CANCEL");

		if (GET_TEAM(GET_PLAYER(getFirstPlayer()).getTeam()).isHasMet(GET_PLAYER(getSecondPlayer()).getTeam()))
		{

			szDealString.clear();
			GAMETEXT.getDealString(szDealString, *this, getFirstPlayer());
			szString.Format(L"%s: %s", szCancelString.GetCString(), szDealString.getCString());
			AddDLLMessage((PlayerTypes)getFirstPlayer(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_DEAL_CANCELLED");
		}

		if (GET_TEAM(GET_PLAYER(getSecondPlayer()).getTeam()).isHasMet(GET_PLAYER(getFirstPlayer()).getTeam()))
		{

			szDealString.clear();
			GAMETEXT.getDealString(szDealString, *this, getSecondPlayer());
			szString.Format(L"%s: %s", szCancelString.GetCString(), szDealString.getCString());
			AddDLLMessage((PlayerTypes)getSecondPlayer(), true, GC.getEVENT_MESSAGE_TIME(), szString, "AS2D_DEAL_CANCELLED");
		}
	}

	CLLNode<TradeData>* pNode;

	for (pNode = headFirstTradesNode(); (pNode != NULL); pNode = nextFirstTradesNode(pNode))
	{
		endTrade(pNode->m_data, getFirstPlayer(), getSecondPlayer(), bKillTeam);
	}

	for (pNode = headSecondTradesNode(); (pNode != NULL); pNode = nextSecondTradesNode(pNode))
	{
		endTrade(pNode->m_data, getSecondPlayer(), getFirstPlayer(), bKillTeam);
	}

	GC.getGame().deleteDeal(getID());
}


void CvDeal::addTrades(CLinkList<TradeData>* pFirstList, CLinkList<TradeData>* pSecondList, bool bCheckAllowed)
{
	if (isVassalTrade(pFirstList) && isVassalTrade(pSecondList))
	{
		return;
	}
	CLLNode<TradeData>* pNode;

	//Afforess: Prevent humans from gifting AI's "negative" value deals
	int iHumanValue = 0;
	if (pFirstList != NULL && bCheckAllowed)
	{
		for (pNode = pFirstList->head(); pNode; pNode = pFirstList->next(pNode))
		{
			if (!GET_PLAYER(getFirstPlayer()).canTradeItem(getSecondPlayer(), pNode->m_data))
			{
				return;
			}
			if (GET_PLAYER(getFirstPlayer()).isHuman())
			{
				iHumanValue += GET_PLAYER(getSecondPlayer()).AI_dealVal(getFirstPlayer(), pFirstList, true, 0);
			}
		}
	}

	if (iHumanValue < 0)
	{
		return;
	}

	if (pSecondList != NULL && bCheckAllowed)
	{
		for (pNode = pSecondList->head(); pNode; pNode = pSecondList->next(pNode))
		{
			if (!GET_PLAYER(getSecondPlayer()).canTradeItem(getFirstPlayer(), pNode->m_data))
			{
				return;
			}
		}
	}

	TeamTypes eFirstTeam = GET_PLAYER(getFirstPlayer()).getTeam();
	TeamTypes eSecondTeam = GET_PLAYER(getSecondPlayer()).getTeam();

	if (atWar(eFirstTeam, eSecondTeam))
	{
		// free vassals of capitulating team before peace is signed
		if (isVassalTrade(pSecondList))
		{
			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				if (iI != eFirstTeam && iI != eSecondTeam
				&& GET_TEAM((TeamTypes)iI).isAlive()
				&& GET_TEAM((TeamTypes)iI).isVassal(eSecondTeam))
				{
					GET_TEAM(eSecondTeam).freeVassal((TeamTypes)iI);
					GET_TEAM(eFirstTeam).AI_setWarSuccess
					(
						(TeamTypes)iI,
						std::max(
							(
								GET_TEAM(eFirstTeam).AI_getWarSuccess(eSecondTeam)
								+ GC.getWAR_SUCCESS_CITY_CAPTURING()
								* GET_TEAM(eSecondTeam).getNumCities()
							),
							GET_TEAM(eFirstTeam).AI_getWarSuccess((TeamTypes)iI)
						)
					);
				}
			}
		}

		if (isVassalTrade(pFirstList))
		{
			for (int iI = 0; iI < MAX_PC_TEAMS; iI++)
			{
				if (iI != eFirstTeam && iI != eSecondTeam
				&& GET_TEAM((TeamTypes)iI).isAlive()
				&& GET_TEAM((TeamTypes)iI).isVassal(eFirstTeam))
				{
					GET_TEAM(eFirstTeam).freeVassal((TeamTypes)iI);
					GET_TEAM(eSecondTeam).AI_setWarSuccess
					(
						(TeamTypes)iI,
						std::max(
							(
								GET_TEAM(eSecondTeam).AI_getWarSuccess(eFirstTeam)
								+ GC.getWAR_SUCCESS_CITY_CAPTURING()
								* GET_TEAM(eFirstTeam).getNumCities()
							),
							GET_TEAM(eSecondTeam).AI_getWarSuccess((TeamTypes)iI)
						)
					);
				}
			}
		}
		GET_TEAM(eFirstTeam).makePeace(eSecondTeam, !isVassalTrade(pFirstList) && !isVassalTrade(pSecondList));
	}
	else if (!isPeaceDealBetweenOthers(pFirstList, pSecondList))
	{
		if (pSecondList != NULL && pSecondList->getLength() > 0)
		{
			if (pFirstList != NULL && pFirstList->getLength() > 0)
			{
				GET_PLAYER(getFirstPlayer()).AI_changePeacetimeTradeValue
				(
					getSecondPlayer(),
					GET_PLAYER(getFirstPlayer()).AI_dealVal(getSecondPlayer(), pSecondList, true)
				);
				GET_PLAYER(getSecondPlayer()).AI_invalidateAttitudeCache(getFirstPlayer());
			}
			else
			{
				GET_PLAYER(getFirstPlayer()).AI_changePeacetimeGrantValue
				(
					getSecondPlayer(),
					GET_PLAYER(getFirstPlayer()).AI_dealVal(getSecondPlayer(), pSecondList, true)
				);
			}
			GET_PLAYER(getFirstPlayer()).AI_invalidateAttitudeCache(getSecondPlayer());
		}
		if (pFirstList != NULL && pFirstList->getLength() > 0)
		{
			if (pSecondList != NULL && pSecondList->getLength() > 0)
			{
				GET_PLAYER(getSecondPlayer()).AI_changePeacetimeTradeValue
				(
					getFirstPlayer(),
					GET_PLAYER(getSecondPlayer()).AI_dealVal(getFirstPlayer(), pFirstList, true)
				);
				GET_PLAYER(getFirstPlayer()).AI_invalidateAttitudeCache(getSecondPlayer());
			}
			else
			{
				GET_PLAYER(getSecondPlayer()).AI_changePeacetimeGrantValue
				(
					getFirstPlayer(),
					GET_PLAYER(getSecondPlayer()).AI_dealVal(getFirstPlayer(), pFirstList, true)
				);
			}
			GET_PLAYER(getSecondPlayer()).AI_invalidateAttitudeCache(getFirstPlayer());
		}
	}

	if (pFirstList != NULL)
	{
		for (pNode = pFirstList->head(); pNode; pNode = pFirstList->next(pNode))
		{
			if (startTrade(pNode->m_data, getFirstPlayer(), getSecondPlayer()))
			{
				insertAtEndFirstTrades(pNode->m_data);
			}
		}
	}

	if (pSecondList != NULL)
	{
		for (pNode = pSecondList->head(); pNode; pNode = pSecondList->next(pNode))
		{
			if (startTrade(pNode->m_data, getSecondPlayer(), getFirstPlayer()))
			{
				insertAtEndSecondTrades(pNode->m_data);
			}
		}
	}

	bool bAlliance = false;

	if (pFirstList != NULL)
	{
		for (pNode = pFirstList->head(); pNode; pNode = pFirstList->next(pNode))
		{
			if (pNode->m_data.m_eItemType == TRADE_PERMANENT_ALLIANCE)
			{
				bAlliance = true;
				break;
			}
		}
	}

	if (!bAlliance && pSecondList != NULL)
	{
		for (pNode = pSecondList->head(); pNode; pNode = pSecondList->next(pNode))
		{
			if (pNode->m_data.m_eItemType == TRADE_PERMANENT_ALLIANCE)
			{
				bAlliance = true;
				break;
			}
		}
	}

	if (bAlliance)
	{
		if (eFirstTeam < eSecondTeam)
		{
			GET_TEAM(eFirstTeam).addTeam(eSecondTeam);
		}
		else if (eSecondTeam < eFirstTeam)
		{
			GET_TEAM(eSecondTeam).addTeam(eFirstTeam);
		}
	}
}


void CvDeal::doTurn()
{
	if (isPeaceDeal())
	{
		return;
	}
	if (getLengthSecondTrades() > 0)
	{
		if (getLengthFirstTrades() > 0)
		{
			GET_PLAYER(getFirstPlayer()).AI_changePeacetimeTradeValue
			(
				getSecondPlayer(),
				GET_PLAYER(getFirstPlayer()).AI_dealVal(getSecondPlayer(), getSecondTrades()) / getTreatyLength()
			);
		}
		else
		{
			GET_PLAYER(getFirstPlayer()).AI_changePeacetimeGrantValue
			(
				getSecondPlayer(),
				GET_PLAYER(getFirstPlayer()).AI_dealVal(getSecondPlayer(), getSecondTrades()) / getTreatyLength()
			);
		}
	}
	if (getLengthFirstTrades() > 0)
	{
		if (getLengthSecondTrades() > 0)
		{
			GET_PLAYER(getSecondPlayer()).AI_changePeacetimeTradeValue
			(
				getFirstPlayer(),
				GET_PLAYER(getSecondPlayer()).AI_dealVal(getFirstPlayer(), getFirstTrades()) / getTreatyLength()
			);
		}
		else
		{
			GET_PLAYER(getSecondPlayer()).AI_changePeacetimeGrantValue
			(
				getFirstPlayer(),
				GET_PLAYER(getSecondPlayer()).AI_dealVal(getFirstPlayer(), getFirstTrades()) / getTreatyLength()
			);
		}
	}
}


// XXX probably should have some sort of message for the user or something...
void CvDeal::verify()
{
	bool bCancelDeal = false;

	CvPlayer& kFirstPlayer = GET_PLAYER(getFirstPlayer());
	CvPlayer& kSecondPlayer = GET_PLAYER(getSecondPlayer());

	for (CLLNode<TradeData>* pNode = headFirstTradesNode(); (pNode != NULL); pNode = nextFirstTradesNode(pNode))
	{
		if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
		{
			// XXX embargoes?
			if ((kFirstPlayer.getNumTradeableBonuses((BonusTypes)(pNode->m_data.m_iData)) < 0) ||
				  !(kFirstPlayer.canTradeNetworkWith(getSecondPlayer())) ||
				  GET_TEAM(kFirstPlayer.getTeam()).isBonusObsolete((BonusTypes) pNode->m_data.m_iData) ||
				  GET_TEAM(kSecondPlayer.getTeam()).isBonusObsolete((BonusTypes) pNode->m_data.m_iData))
			{
				bCancelDeal = true;
			}
		}
	}

	for (CLLNode<TradeData>* pNode = headSecondTradesNode(); (pNode != NULL); pNode = nextSecondTradesNode(pNode))
	{
		if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
		{
			// XXX embargoes?
			if ((GET_PLAYER(getSecondPlayer()).getNumTradeableBonuses((BonusTypes)(pNode->m_data.m_iData)) < 0) ||
				  !(GET_PLAYER(getSecondPlayer()).canTradeNetworkWith(getFirstPlayer())) ||
				  GET_TEAM(kFirstPlayer.getTeam()).isBonusObsolete((BonusTypes) pNode->m_data.m_iData) ||
				  GET_TEAM(kSecondPlayer.getTeam()).isBonusObsolete((BonusTypes) pNode->m_data.m_iData))
			{
				bCancelDeal = true;
			}
		}
	}

	if (isCancelable(NO_PLAYER))
	{
		if (isPeaceDeal())
		{
			bCancelDeal = true;
		}
	}

	if (bCancelDeal)
	{
		kill();
	}
}


bool CvDeal::isPeaceDeal() const
{
	CLLNode<TradeData>* pNode;

	for (pNode = headFirstTradesNode(); (pNode != NULL); pNode = nextFirstTradesNode(pNode))
	{
		if (pNode->m_data.m_eItemType == getPeaceItem())
		{
			return true;
		}
	}

	for (pNode = headSecondTradesNode(); (pNode != NULL); pNode = nextSecondTradesNode(pNode))
	{
		if (pNode->m_data.m_eItemType == getPeaceItem())
		{
			return true;
		}
	}

	return false;
}

bool CvDeal::isVassalDeal() const
{
	return (isVassalTrade(&m_firstTrades) || isVassalTrade(&m_secondTrades));
}

bool CvDeal::isVassalTrade(const CLinkList<TradeData>* pList)
{
	if (pList)
	{
		for (CLLNode<TradeData>* pNode = pList->head(); pNode != NULL; pNode = pList->next(pNode))
		{
			if (isVassal(pNode->m_data.m_eItemType))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvDeal::isUncancelableVassalDeal(PlayerTypes eByPlayer, CvWString* pszReason) const
{
	for (CLLNode<TradeData>* pNode = headFirstTradesNode(); pNode != NULL; pNode = nextFirstTradesNode(pNode))
	{
		if (isVassal(pNode->m_data.m_eItemType) && eByPlayer == getSecondPlayer())
		{
			if (pszReason)
			{
				*pszReason += gDLL->getText("TXT_KEY_MISC_DEAL_NO_CANCEL_EVER");
			}
			return true;
		}

		if (pNode->m_data.m_eItemType == TRADE_SURRENDER)
		{
			const CvTeam& kVassal = GET_TEAM(GET_PLAYER(getFirstPlayer()).getTeam());
			TeamTypes eMaster = GET_PLAYER(getSecondPlayer()).getTeam();

			if (!kVassal.canVassalRevolt(eMaster))
			{
				if (pszReason)
				{
					CvWStringBuffer szBuffer;
					GAMETEXT.setVassalRevoltHelp(szBuffer, eMaster, GET_PLAYER(getFirstPlayer()).getTeam());
					*pszReason = szBuffer.getCString();
				}
				return true;
			}
		}
	}

	for (CLLNode<TradeData>* pNode = headSecondTradesNode(); (pNode != NULL); pNode = nextSecondTradesNode(pNode))
	{
		if (isVassal(pNode->m_data.m_eItemType) && eByPlayer == getFirstPlayer())
		{
			if (pszReason)
			{
				*pszReason += gDLL->getText("TXT_KEY_MISC_DEAL_NO_CANCEL_EVER");
			}
			return true;
		}

		if (pNode->m_data.m_eItemType == TRADE_SURRENDER)
		{
			CvTeam& kVassal = GET_TEAM(GET_PLAYER(getSecondPlayer()).getTeam());
			TeamTypes eMaster = GET_PLAYER(getFirstPlayer()).getTeam();

			if (!kVassal.canVassalRevolt(eMaster))
			{
				if (pszReason)
				{
					CvWStringBuffer szBuffer;
					GAMETEXT.setVassalRevoltHelp(szBuffer, eMaster, GET_PLAYER(getFirstPlayer()).getTeam());
					*pszReason = szBuffer.getCString();
				}
				return true;
			}
		}
	}
	return false;
}

bool CvDeal::isVassalTributeDeal(const CLinkList<TradeData>* pList)
{
	for (CLLNode<TradeData>* pNode = pList->head(); pNode != NULL; pNode = pList->next(pNode))
	{
		if (pNode->m_data.m_eItemType != TRADE_RESOURCES)
		{
			return false;
		}
	}
	return true;
}

bool CvDeal::isPeaceDealBetweenOthers(CLinkList<TradeData>* pFirstList, CLinkList<TradeData>* pSecondList) const
{
	CLLNode<TradeData>* pNode;

	if (pFirstList != NULL)
	{
		for (pNode = pFirstList->head(); pNode; pNode = pFirstList->next(pNode))
		{
			if (pNode->m_data.m_eItemType == TRADE_PEACE)
			{
				return true;
			}
		}
	}

	if (pSecondList != NULL)
	{
		for (pNode = pSecondList->head(); pNode; pNode = pSecondList->next(pNode))
		{
			if (pNode->m_data.m_eItemType == TRADE_PEACE)
			{
				return true;
			}
		}
	}
	return false;
}


int CvDeal::getID() const
{
	return m_iID;
}


void CvDeal::setID(int iID)
{
	m_iID = iID;
}


int CvDeal::getInitialGameTurn() const
{
	return m_iInitialGameTurn;
}


void CvDeal::setInitialGameTurn(int iNewValue)
{
	m_iInitialGameTurn = iNewValue;
}


PlayerTypes CvDeal::getFirstPlayer() const
{
	return m_eFirstPlayer;
}


PlayerTypes CvDeal::getSecondPlayer() const
{
	return m_eSecondPlayer;
}


void CvDeal::clearFirstTrades()
{
	m_firstTrades.clear();
}


void CvDeal::insertAtEndFirstTrades(TradeData trade)
{
	m_firstTrades.insertAtEnd(trade);
}


CLLNode<TradeData>* CvDeal::nextFirstTradesNode(CLLNode<TradeData>* pNode) const
{
	return m_firstTrades.next(pNode);
}

void CvDeal::removeFirstTradeNode(CLLNode<TradeData>* pNode)
{
	m_firstTrades.deleteNode(pNode);
}

int CvDeal::getLengthFirstTrades() const
{
	return m_firstTrades.getLength();
}


CLLNode<TradeData>* CvDeal::headFirstTradesNode() const
{
	return m_firstTrades.head();
}


const CLinkList<TradeData>* CvDeal::getFirstTrades() const
{
	return &(m_firstTrades);
}


void CvDeal::clearSecondTrades()
{
	m_secondTrades.clear();
}


void CvDeal::insertAtEndSecondTrades(TradeData trade)
{
	m_secondTrades.insertAtEnd(trade);
}


CLLNode<TradeData>* CvDeal::nextSecondTradesNode(CLLNode<TradeData>* pNode) const
{
	return m_secondTrades.next(pNode);
}

void CvDeal::removeSecondTradeNode(CLLNode<TradeData>* pNode)
{
	m_secondTrades.deleteNode(pNode);
}


int CvDeal::getLengthSecondTrades() const
{
	return m_secondTrades.getLength();
}


CLLNode<TradeData>* CvDeal::headSecondTradesNode() const
{
	return m_secondTrades.head();
}


const CLinkList<TradeData>* CvDeal::getSecondTrades() const
{
	return &(m_secondTrades);
}


void CvDeal::write(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvDeal", m_iID);
	WRAPPER_WRITE(wrapper, "CvDeal", m_iInitialGameTurn);

	WRAPPER_WRITE(wrapper, "CvDeal", m_eFirstPlayer);
	WRAPPER_WRITE(wrapper, "CvDeal", m_eSecondPlayer);

	m_firstTrades.Write(pStream);
	m_secondTrades.Write(pStream);

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void CvDeal::read(FDataStreamBase* pStream)
{
	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	WRAPPER_READ(wrapper,"CvDeal",&m_iID);
	WRAPPER_READ(wrapper,"CvDeal",&m_iInitialGameTurn);

	WRAPPER_READ(wrapper,"CvDeal",(int*)&m_eFirstPlayer);
	WRAPPER_READ(wrapper,"CvDeal",(int*)&m_eSecondPlayer);

	m_firstTrades.Read(pStream);
	m_secondTrades.Read(pStream);

	if (wrapper.isUsingTaggedFormat())
	{
		CLLNode<TradeData>* pNode;
		CLLNode<TradeData>* pNextNode;

		for (pNode = headFirstTradesNode(); (pNode != NULL); pNode = pNextNode)
		{
			pNextNode = nextFirstTradesNode(pNode);

			if (!TranslateTradeDataOnLoad(wrapper, pNode->m_data))
			{
				removeFirstTradeNode(pNode);
			}
		}

		for (pNode = headSecondTradesNode(); (pNode != NULL); pNode = pNextNode)
		{
			pNextNode = nextSecondTradesNode(pNode);

			if (!TranslateTradeDataOnLoad(wrapper, pNode->m_data))
			{
				removeSecondTradeNode(pNode);
			}
		}
	}

	WRAPPER_READ_OBJECT_END(wrapper);
}

// Protected Functions...

//	Translate from save-space to load-space enums in trade deal infos
bool CvDeal::TranslateTradeDataOnLoad(CvTaggedSaveFormatWrapper& wrapper, TradeData& data)
{
	if (data.m_eItemType == TRADE_RESOURCES)
	{
		data.m_iData =  wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BONUSES, data.m_iData, true);

		return (data.m_iData != NO_BONUS);
	}
	return true;
}

// Returns true if the trade should be saved...
bool CvDeal::startTrade(TradeData trade, PlayerTypes eFromPlayer, PlayerTypes eToPlayer)
{
	bool bSave = false;

	switch (trade.m_eItemType)
	{
		case TRADE_TECHNOLOGIES:
		{
			GET_TEAM(GET_PLAYER(eToPlayer).getTeam()).setHasTech(((TechTypes)trade.m_iData), true, eToPlayer, true, true);
			if (GC.getGame().isOption(GAMEOPTION_NO_TECH_BROKERING))
			{
				GET_TEAM(GET_PLAYER(eToPlayer).getTeam()).setNoTradeTech(((TechTypes)trade.m_iData), true);
			}

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) trades tech %S due to TRADE_TECHNOLOGIES with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), GC.getTechInfo((TechTypes)trade.m_iData).getDescription(), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == GET_PLAYER(eToPlayer).getTeam())
					{
						GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(eFromPlayer, MEMORY_TRADED_TECH_TO_US, 1);
					}
					else if (GET_TEAM(GET_PLAYER((PlayerTypes)iI).getTeam()).isHasMet(GET_PLAYER(eToPlayer).getTeam()))
					{
						GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(eToPlayer, MEMORY_RECEIVED_TECH_FROM_ANY, 1);
					}
				}
			}
			break;
		}
		case TRADE_RESOURCES:
		{
			GET_PLAYER(eFromPlayer).changeBonusExport(((BonusTypes)trade.m_iData), 1);
			GET_PLAYER(eToPlayer).changeBonusImport(((BonusTypes)trade.m_iData), 1);

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) trades bonus type %S due to TRADE_RESOURCES with %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), GC.getBonusInfo((BonusTypes)trade.m_iData).getDescription(), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			bSave = true;
			break;
		}
		case TRADE_CITIES:
		{
			CvCity* pCity = GET_PLAYER(eFromPlayer).getCity(trade.m_iData);
			if (pCity != NULL)
			{
				if (gTeamLogLevel >= 2)
				{
					logBBAI("    Player %d (%S) gives a city due to TRADE_CITIES with %d (%S)",
						eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
				}
				pCity->doTask(TASK_GIFT, eToPlayer);
			}
			break;
		}
		case TRADE_WORKER:
		case TRADE_MILITARY_UNIT:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				CvUnit* pUnit = GET_PLAYER(eFromPlayer).getUnit(trade.m_iData);
				if (pUnit != NULL)
				{
					pUnit->tradeUnit(eToPlayer);
				}
			}
			break;
		}
		case TRADE_GOLD:
		{
			GET_PLAYER(eFromPlayer).changeGold(-(trade.m_iData));
			GET_PLAYER(eToPlayer).changeGold(trade.m_iData);
			GET_PLAYER(eFromPlayer).AI_changeGoldTradedTo(eToPlayer, trade.m_iData);

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) trades gold %d due to TRADE_GOLD with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			CvEventReporter::getInstance().playerGoldTrade(eFromPlayer, eToPlayer, trade.m_iData);
			break;
		}

		case TRADE_GOLD_PER_TURN:
		{
			GET_PLAYER(eFromPlayer).changeGoldPerTurnByPlayer(eToPlayer, -(trade.m_iData));
			GET_PLAYER(eToPlayer).changeGoldPerTurnByPlayer(eFromPlayer, trade.m_iData);

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) trades gold per turn %d due to TRADE_GOLD_PER_TURN with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			bSave = true;
			break;
		}
		case TRADE_MAPS:
		{
			for (int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot* pLoopPlot = GC.getMap().plotByIndex(iI);

				if (pLoopPlot->isRevealed(GET_PLAYER(eFromPlayer).getTeam(), false))
				{
					pLoopPlot->setRevealed(GET_PLAYER(eToPlayer).getTeam(), true, false, GET_PLAYER(eFromPlayer).getTeam(), false);
				}
			}

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(GET_PLAYER(eToPlayer).getTeam()))
				{
					GET_PLAYER((PlayerTypes)iI).updatePlotGroups();
				}
			}

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) trades maps due to TRADE_MAPS with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			break;
		}
		case TRADE_SURRENDER:
		case TRADE_VASSAL:
		{
			if (trade.m_iData == 0)
			{
				startTeamTrade(trade.m_eItemType, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam(), false);
				GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setVassal(GET_PLAYER(eToPlayer).getTeam(), true, TRADE_SURRENDER == trade.m_eItemType);

				if (gTeamLogLevel >= 2)
				{
					if (TRADE_SURRENDER == trade.m_eItemType)
					{
						logBBAI("    Player %d (%S) trades themselves as vassal due to TRADE_SURRENDER with player %d (%S)",
							eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
					}
					else
					{
						logBBAI("    Player %d (%S) trades themselves as vassal due to TRADE_VASSAL with player %d (%S)",
							eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
					}
				}
			}
			else
			{
				bSave = true;
			}
			break;
		}
		case TRADE_PEACE:
		{
			CvTeam& fromTeam = GET_TEAM(GET_PLAYER(eFromPlayer).getTeam());

			fromTeam.makePeace((TeamTypes)trade.m_iData);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && fromTeam.isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(eFromPlayer, MEMORY_MADE_PEACE, 1);
				}
			}
			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Team %d (%S) makes peace with team %d due to TRADE_PEACE with %d (%S)",
					GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eFromPlayer).getCivilizationDescription(0), trade.m_iData, eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			break;
		}
		case TRADE_WAR:
		{
			const TeamTypes eToTeam = GET_PLAYER(eToPlayer).getTeam(); // War mongerer
			const TeamTypes eFromTeam = GET_PLAYER(eFromPlayer).getTeam(); // War ally
			const TeamTypes eVictimTeam = (TeamTypes)trade.m_iData;

			GET_TEAM(eFromTeam).declareWar(eVictimTeam, true, NO_WARPLAN);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				const TeamTypes eTeamX = GET_PLAYER((PlayerTypes)iI).getTeam();
				// If alive and not the war mongerer/ally.
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eFromTeam, false, eToTeam))
				{
					// If victim, or have met the war ally purchaser and the victim.
					if (eTeamX == eVictimTeam
					|| GET_TEAM(eTeamX).isHasMet(eToTeam)
					&& GET_TEAM(eTeamX).isHasMet(eVictimTeam)
					// If not at war with the victim (war allies are good!)
					&& !GET_TEAM(eTeamX).isAtWar(eVictimTeam))
					{
						// Remember war ally purchaser as war mongerer
						GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(eToPlayer, MEMORY_WARMONGER, 1);
					}
					if (eTeamX == eVictimTeam)
					{
						AddDLLMessage(
							(PlayerTypes)iI, true, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_HIRED_WAR_ALLY",
								GET_PLAYER(eToPlayer).getCivilizationAdjectiveKey(),
								GET_PLAYER(eFromPlayer).getCivilizationAdjectiveKey()
							)
							, "AS2D_BUILD_BARRACKS", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_RED()
						);
						GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(eToPlayer, MEMORY_HIRED_WAR_ALLY, 1);
					}
				}
			}
			break;
		}
		case TRADE_EMBARGO:
		{
			GET_PLAYER(eFromPlayer).stopTradingWithTeam((TeamTypes)trade.m_iData);

			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam((TeamTypes)trade.m_iData))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(eToPlayer, MEMORY_HIRED_TRADE_EMBARGO, 1);
				}
			}

			if (gTeamLogLevel >= 2)
			{
				logBBAI(
					"    Player %d (%S) signs embargo against team %d due to TRADE_EMBARGO with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), (TeamTypes)trade.m_iData,
					eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0)
				);
			}
			break;
		}
		case TRADE_CIVIC:
		{
			std::vector<CivicTypes> paeNewCivics(GC.getNumCivicOptionInfos(), NO_CIVIC);
			for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
			{
				paeNewCivics[iI] = GET_PLAYER(eFromPlayer).getCivics((CivicOptionTypes)iI);
			}

			paeNewCivics[GC.getCivicInfo((CivicTypes)trade.m_iData).getCivicOptionType()] = ((CivicTypes)trade.m_iData);

			GET_PLAYER(eFromPlayer).revolution(&paeNewCivics[0], true);

			if (GET_PLAYER(eFromPlayer).AI_getCivicTimer() < getTreatyLength())
			{
				GET_PLAYER(eFromPlayer).AI_setCivicTimer(getTreatyLength());
			}

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) switched civics due to TRADE_CIVICS with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			break;
		}
		case TRADE_RELIGION:
		{
			GET_PLAYER(eFromPlayer).convert((ReligionTypes)trade.m_iData);

			if (GET_PLAYER(eFromPlayer).AI_getReligionTimer() < getTreatyLength())
			{
				GET_PLAYER(eFromPlayer).AI_setReligionTimer(getTreatyLength());
			}

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) switched religions due to TRADE_RELIGION with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			break;
		}
		case TRADE_EMBASSY:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				if (trade.m_iData == 0)
				{
					startTeamTrade(TRADE_EMBASSY, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam(), true);
					GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setHasEmbassy(((TeamTypes)(GET_PLAYER(eToPlayer).getTeam())), true);
				}
				else
				{
					bSave = true;
				}
			}
			break;
		}
		case TRADE_CONTACT:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				GET_TEAM(GET_PLAYER(eToPlayer).getTeam()).meet((TeamTypes)trade.m_iData, false); //Was true...
			}
			break;
		}
		case TRADE_CORPORATION:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				CvPlayer& toPlayer = GET_PLAYER(eToPlayer);
				CvPlayer& fromPlayer = GET_PLAYER(eFromPlayer);

				CvCity* pOldHeadquarters = GC.getGame().getHeadquarters((CorporationTypes)trade.m_iData);
				CvCity* pNewHQCity = toPlayer.getBestHQCity((CorporationTypes)trade.m_iData);
				pNewHQCity->setHasCorporation((CorporationTypes)trade.m_iData, true, false, false);
				GC.getGame().setHeadquarters((CorporationTypes)trade.m_iData, pNewHQCity, true);
				//Move the HQ building over to the new city.
				for (int i = 0; i < GC.getNumBuildingInfos(); i++)
				{
					if ((CorporationTypes)GC.getBuildingInfo((BuildingTypes)i).getGlobalCorporationCommerce() == (CorporationTypes)trade.m_iData)
					{
						pNewHQCity->setNumRealBuilding((BuildingTypes)i, 1);
						pOldHeadquarters->setNumRealBuilding((BuildingTypes)i, 0);
						break;
					}
				}

				//The old HQ city still has the corporation, just not the HQ.
				if (pOldHeadquarters != NULL)
					pOldHeadquarters->setHasCorporation((CorporationTypes)trade.m_iData, true, false, false);

				toPlayer.updateCorporation();
				fromPlayer.updateCorporation();

				const CvTeam& toTeam = GET_TEAM(toPlayer.getTeam());
				const CvTeam& fromTeam = GET_TEAM(fromPlayer.getTeam());

				for (int i = 0; i < MAX_PC_PLAYERS; i++)
				{
					if (toTeam.isHasMet(GET_PLAYER((PlayerTypes)i).getTeam())
					|| fromTeam.isHasMet(GET_PLAYER((PlayerTypes)i).getTeam()))
					{
						AddDLLMessage(
							(PlayerTypes)i, true, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_CORPORATION_TRADE",
								GC.getCorporationInfo((CorporationTypes)trade.m_iData).getDescription(),
								fromPlayer.getCivilizationDescriptionKey(),
								toPlayer.getCivilizationDescriptionKey()
							).GetCString(),
							"AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, GC.getCOLOR_HIGHLIGHT_TEXT()
						);
					}
				}
			}
			break;
		}
		case TRADE_SECRETARY_GENERAL_VOTE:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				GET_PLAYER(eFromPlayer).setPledgedSecretaryGeneralVote(GET_PLAYER(eToPlayer).getTeam());
			}
			break;
		}
		case TRADE_PLEDGE_VOTE:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				GET_PLAYER(eFromPlayer).setPledgedVote((PlayerVoteTypes)trade.m_iData);
			}
			break;
		}
		case TRADE_OPEN_BORDERS:
		{
			if (trade.m_iData == 0)
			{
				startTeamTrade(TRADE_OPEN_BORDERS, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam(), true);
				GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setOpenBorders(((TeamTypes)(GET_PLAYER(eToPlayer).getTeam())), true);
				endTeamTrade(TRADE_RITE_OF_PASSAGE, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam());
			}
			else
			{
				bSave = true;
			}
			break;
		}
		case TRADE_RITE_OF_PASSAGE:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				if (trade.m_iData == 0)
				{
					startTeamTrade(TRADE_RITE_OF_PASSAGE, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam(), true);
					GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setLimitedBorders(((TeamTypes)(GET_PLAYER(eToPlayer).getTeam())), true);
				}
				else
				{
					bSave = true;
				}
			}
			break;
		}
		case TRADE_FREE_TRADE_ZONE:
		{
			if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
			{
				if (trade.m_iData == 0)
				{
					startTeamTrade(TRADE_FREE_TRADE_ZONE, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam(), true);
					GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setFreeTradeAgreement(((TeamTypes)(GET_PLAYER(eToPlayer).getTeam())), true);
				}
				else
				{
					bSave = true;
				}
			}
			break;
		}
		case TRADE_DEFENSIVE_PACT:
		{
			if (trade.m_iData == 0)
			{
				startTeamTrade(TRADE_DEFENSIVE_PACT, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam(), true);
				GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setDefensivePact(((TeamTypes)(GET_PLAYER(eToPlayer).getTeam())), true);

				if (gTeamLogLevel >= 2)
				{
					logBBAI("    Player %d (%S) signs defensive pact due to TRADE_DEFENSIVE_PACT with player %d (%S)",
						eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
				}
			}
			else
			{
				bSave = true;
			}
			break;
		}
		case TRADE_PERMANENT_ALLIANCE:
			break;
		case TRADE_PEACE_TREATY:
		{
			GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setForcePeace(((TeamTypes)(GET_PLAYER(eToPlayer).getTeam())), true);

			if (gTeamLogLevel >= 2)
			{
				logBBAI("    Player %d (%S) signs peace treaty due to TRADE_PEACE_TREATY with player %d (%S)",
					eFromPlayer, GET_PLAYER(eFromPlayer).getCivilizationDescription(0), eToPlayer, GET_PLAYER(eToPlayer).getCivilizationDescription(0));
			}
			bSave = true;
			break;
		}
		default:
		{
			FErrorMsg("error");
			break;
		}
	}
	return bSave;
}


void CvDeal::endTrade(TradeData trade, PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bTeam)
{
	switch (trade.m_eItemType)
	{
		case TRADE_RESOURCES:
		{
			GET_PLAYER(eToPlayer).changeBonusImport((BonusTypes) trade.m_iData, -1);
			GET_PLAYER(eFromPlayer).changeBonusExport((BonusTypes) trade.m_iData, -1);
			break;
		}
		case TRADE_GOLD_PER_TURN:
		{
			GET_PLAYER(eFromPlayer).changeGoldPerTurnByPlayer(eToPlayer, trade.m_iData);
			GET_PLAYER(eToPlayer).changeGoldPerTurnByPlayer(eFromPlayer, -trade.m_iData);
			break;
		}
		case TRADE_VASSAL:
		{
			GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setVassal(GET_PLAYER(eToPlayer).getTeam(), false, false);
			if (bTeam)
			{
				endTeamTrade(TRADE_VASSAL, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam());
			}
			break;
		}
		case TRADE_SURRENDER:
		{
			GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setVassal(GET_PLAYER(eToPlayer).getTeam(), false, true);
			if (bTeam)
			{
				endTeamTrade(TRADE_SURRENDER, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam());
			}
			break;
		}
		case TRADE_EMBASSY:
		{
			const TeamTypes eToTeam = GET_PLAYER(eToPlayer).getTeam();
			const TeamTypes eFromTeam = GET_PLAYER(eFromPlayer).getTeam();

			GET_TEAM(eFromTeam).setHasEmbassy(eToTeam, false);
			if (bTeam)
			{
				endTeamTrade(TRADE_EMBASSY, eFromTeam, eToTeam);
				endTeamTrade(TRADE_OPEN_BORDERS, eFromTeam, eToTeam);
				endTeamTrade(TRADE_DEFENSIVE_PACT, eFromTeam, eToTeam);
				endTeamTrade(TRADE_FREE_TRADE_ZONE, eFromTeam, eToTeam);
			}
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eFromTeam))
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eToTeam))
						{
							GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount((PlayerTypes)iJ, MEMORY_RECALLED_AMBASSADOR, 1);
						}
					}
				}
			}
			break;
		}
		case TRADE_OPEN_BORDERS:
		{
			const TeamTypes eToTeam = GET_PLAYER(eToPlayer).getTeam();
			const TeamTypes eFromTeam = GET_PLAYER(eFromPlayer).getTeam();

			GET_TEAM(eFromTeam).setOpenBorders(eToTeam, false);
			if (bTeam)
			{
				endTeamTrade(TRADE_OPEN_BORDERS, eFromTeam, eToTeam);
				endTeamTrade(TRADE_RITE_OF_PASSAGE, eFromTeam, eToTeam);
			}
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eToTeam))
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eFromTeam))
						{
							GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount((PlayerTypes)iJ, MEMORY_CANCELLED_OPEN_BORDERS, 1);
						}
					}
				}
			}
			break;
		}
		case TRADE_RITE_OF_PASSAGE:
		{
			const TeamTypes eToTeam = GET_PLAYER(eToPlayer).getTeam();
			const TeamTypes eFromTeam = GET_PLAYER(eFromPlayer).getTeam();

			GET_TEAM(eFromTeam).setLimitedBorders(eToTeam, false);
			if (bTeam)
			{
				endTeamTrade(TRADE_RITE_OF_PASSAGE, eFromTeam, eToTeam);
			}
			for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eToTeam))
				{
					for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eFromTeam))
						{
							GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount((PlayerTypes)iJ, MEMORY_CANCELLED_OPEN_BORDERS, 1);
						}
					}
				}
			}
			break;
		}
		case TRADE_FREE_TRADE_ZONE:
		{
			GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setFreeTradeAgreement(GET_PLAYER(eToPlayer).getTeam(), false);
			if (bTeam)
			{
				endTeamTrade(TRADE_FREE_TRADE_ZONE, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam());
			}
			break;
		}
		case TRADE_DEFENSIVE_PACT:
		{
			GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setDefensivePact(GET_PLAYER(eToPlayer).getTeam(), false);
			if (bTeam)
			{
				endTeamTrade(TRADE_DEFENSIVE_PACT, GET_PLAYER(eFromPlayer).getTeam(), GET_PLAYER(eToPlayer).getTeam());
			}
			break;
		}
		case TRADE_PEACE_TREATY:
		{
			GET_TEAM(GET_PLAYER(eFromPlayer).getTeam()).setForcePeace(GET_PLAYER(eToPlayer).getTeam(), false);
			break;
		}
		default:
		{
			FErrorMsg("error");
			break;
		}
	}
}

void CvDeal::startTeamTrade(TradeableItems eItem, TeamTypes eFromTeam, TeamTypes eToTeam, bool bDual)
{
	for (int iI = 0; iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eFromTeam))
		{
			for (int iJ = 0; iJ < MAX_PC_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eToTeam))
				{
					TradeData item;
					setTradeItem(&item, eItem, 1);
					CLinkList<TradeData> ourList;
					ourList.insertAtEnd(item);
					CLinkList<TradeData> theirList;
					if (bDual)
					{
						theirList.insertAtEnd(item);
					}
					GC.getGame().implementDeal((PlayerTypes)iI, (PlayerTypes)iJ, &ourList, &theirList);
				}
			}
		}
	}
}

void CvDeal::endTeamTrade(TradeableItems eItem, TeamTypes eFromTeam, TeamTypes eToTeam)
{
	CLLNode<TradeData>* pNode;

	foreach_(CvDeal& kLoopDeal, GC.getGame().deals())
	{
		if (&kLoopDeal != this)
		{
			bool bValid = true;

			if (GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == eFromTeam
			&&  GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == eToTeam
			&& kLoopDeal.getFirstTrades())
			{
				for (pNode = kLoopDeal.getFirstTrades()->head(); pNode; pNode = kLoopDeal.getFirstTrades()->next(pNode))
				{
					if (pNode->m_data.m_eItemType == eItem)
					{
						bValid = false;
						break;
					}
				}
			}
			if (bValid
			&& GET_PLAYER(kLoopDeal.getFirstPlayer()).getTeam() == eToTeam
			&& GET_PLAYER(kLoopDeal.getSecondPlayer()).getTeam() == eFromTeam
			&& kLoopDeal.getSecondTrades())
			{
				for (pNode = kLoopDeal.getSecondTrades()->head(); pNode; pNode = kLoopDeal.getSecondTrades()->next(pNode))
				{
					if (pNode->m_data.m_eItemType == eItem)
					{
						bValid = false;
						break;
					}
				}
			}
			if (!bValid)
			{
				kLoopDeal.kill(false);
			}
		}
	}
}

bool CvDeal::isCancelable(PlayerTypes eByPlayer, CvWString* pszReason)
{
	if (isUncancelableVassalDeal(eByPlayer, pszReason))
	{
		return false;
	}

	const int iTurns = turnsToCancel();
	if (pszReason)
	{
		if (iTurns > 0)
		{
			*pszReason = gDLL->getText("TXT_KEY_MISC_DEAL_NO_CANCEL", iTurns);
		}
	}

	return (iTurns <= 0);
}

int CvDeal::turnsToCancel() const
{
	return (getInitialGameTurn() + getTreatyLength() - GC.getGame().getGameTurn());
}

// static
bool CvDeal::isAnnual(TradeableItems eItem)
{
	switch (eItem)
	{
	case TRADE_RESOURCES:
	case TRADE_GOLD_PER_TURN:
	case TRADE_VASSAL:
	case TRADE_SURRENDER:
	case TRADE_OPEN_BORDERS:
	case TRADE_DEFENSIVE_PACT:
	case TRADE_PERMANENT_ALLIANCE:
/************************************************************************************************/
/* Afforess	                  Start		 06/16/10                                               */
/*                                                                                              */
/* Advanced Diplomacy                                                                           */
/************************************************************************************************/
	case TRADE_RITE_OF_PASSAGE:
	case TRADE_FREE_TRADE_ZONE:
	case TRADE_EMBASSY:
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		return true;
		break;
	}

	return false;
}

// static
bool CvDeal::isDual(TradeableItems eItem, bool bExcludePeace)
{
	switch (eItem)
	{
	case TRADE_OPEN_BORDERS:
	case TRADE_DEFENSIVE_PACT:
	case TRADE_PERMANENT_ALLIANCE:
/************************************************************************************************/
/* Afforess	                  Start		 06/16/10                                               */
/*                                                                                              */
/* Advanced Diplomacy                                                                           */
/************************************************************************************************/
	case TRADE_EMBASSY:
	case TRADE_RITE_OF_PASSAGE:
	case TRADE_FREE_TRADE_ZONE:
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		return true;
	case TRADE_PEACE_TREATY:
		return (!bExcludePeace);
	}

	return false;
}

// static
bool CvDeal::hasData(TradeableItems eItem)
{
/************************************************************************************************/
/* Afforess	                  Start		 07/17/10                                               */
/*                                                                                              */
/* Advanced Diplomacy                                                                           */
/************************************************************************************************/
	if (GC.getGame().isOption(GAMEOPTION_ADVANCED_DIPLOMACY))
	{
		return (eItem != TRADE_MAPS &&
			eItem != TRADE_VASSAL &&
			eItem != TRADE_SURRENDER &&
			eItem != TRADE_OPEN_BORDERS &&
			eItem != TRADE_DEFENSIVE_PACT &&
			eItem != TRADE_PERMANENT_ALLIANCE &&
			eItem != TRADE_PEACE_TREATY &&
			eItem != TRADE_EMBASSY);
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	return (eItem != TRADE_MAPS &&
		eItem != TRADE_VASSAL &&
		eItem != TRADE_SURRENDER &&
		eItem != TRADE_OPEN_BORDERS &&
		eItem != TRADE_DEFENSIVE_PACT &&
		eItem != TRADE_PERMANENT_ALLIANCE &&
		eItem != TRADE_PEACE_TREATY);
}

bool CvDeal::isGold(TradeableItems eItem)
{
	return (eItem == getGoldItem() || eItem == getGoldPerTurnItem());
}

bool CvDeal::isVassal(TradeableItems eItem)
{
	return (eItem == TRADE_VASSAL || eItem == TRADE_SURRENDER);
}

bool CvDeal::isEndWar(TradeableItems eItem)
{
	if (eItem == getPeaceItem())
	{
		return true;
	}

	if (isVassal(eItem))
	{
		return true;
	}

	return false;
}

TradeableItems CvDeal::getPeaceItem()
{
	return TRADE_PEACE_TREATY;
}

TradeableItems CvDeal::getGoldItem()
{
	return TRADE_GOLD;
}

TradeableItems CvDeal::getGoldPerTurnItem()
{
	return TRADE_GOLD_PER_TURN;
}


bool CvDeal::isSingleOption(TradeableItems eItem)
{
	return (eItem == TRADE_PLEDGE_VOTE);
}


bool CvDeal::isEmbassy() const
{
	CLLNode<TradeData>* pNode;

	for (pNode = headFirstTradesNode(); (pNode != NULL); pNode = nextFirstTradesNode(pNode))
	{
		if (pNode->m_data.m_eItemType == TRADE_EMBASSY)
		{
			return true;
		}
	}

	for (pNode = headSecondTradesNode(); (pNode != NULL); pNode = nextSecondTradesNode(pNode))
	{
		if (pNode->m_data.m_eItemType == TRADE_EMBASSY)
		{
			return true;
		}
	}
	return false;
}
