#pragma once

// CvDeal.h

#ifndef CIV4_DEAL_H
#define CIV4_DEAL_H

#include "LinkedList.h"
//#include "CvStructs.h"

class CvDeal : bst::noncopyable
{
public:
	CvDeal();
	virtual ~CvDeal();

	void init(int iID, PlayerTypes eFirstPlayer, PlayerTypes eSecondPlayer);
	void uninit();
	void reset(int iID = 0, PlayerTypes eFirstPlayer = NO_PLAYER, PlayerTypes eSecondPlayer = NO_PLAYER);
	DllExport void kill(bool bKillTeam = true);

	void addTrades(CLinkList<TradeData>* pFirstList, CLinkList<TradeData>* pSecondList, bool bCheckAllowed);

	void doTurn();

	void verify();

	bool isPeaceDeal() const;
	bool isPeaceDealBetweenOthers(CLinkList<TradeData>* pFirstList, CLinkList<TradeData>* pSecondList) const;
	bool isVassalDeal() const;
	bool isUncancelableVassalDeal(PlayerTypes eByPlayer, CvWString* pszReason = NULL) const;
	DllExport static bool isVassalTributeDeal(const CLinkList<TradeData>* pList);

	DllExport int getID() const;
	void setID(int iID);

	int getInitialGameTurn() const;
	void setInitialGameTurn(int iNewValue);

	DllExport PlayerTypes getFirstPlayer() const;
	DllExport PlayerTypes getSecondPlayer() const;

	void clearFirstTrades();
	void insertAtEndFirstTrades(TradeData trade);
	DllExport CLLNode<TradeData>* nextFirstTradesNode(CLLNode<TradeData>* pNode) const;
	void removeFirstTradeNode(CLLNode<TradeData>* node);
	int getLengthFirstTrades() const;
	DllExport CLLNode<TradeData>* headFirstTradesNode() const;
	const CLinkList<TradeData>* getFirstTrades() const;

	void clearSecondTrades();
	void insertAtEndSecondTrades(TradeData trade);
	DllExport CLLNode<TradeData>* nextSecondTradesNode(CLLNode<TradeData>* pNode) const;
	void removeSecondTradeNode(CLLNode<TradeData>* node);
	int getLengthSecondTrades() const;
	DllExport CLLNode<TradeData>* headSecondTradesNode() const;
	const CLinkList<TradeData>* getSecondTrades() const;

	DllExport bool isCancelable(PlayerTypes eByPlayer = NO_PLAYER, CvWString* pszReason = NULL);
	int turnsToCancel(PlayerTypes eByPlayer = NO_PLAYER);

	static bool isAnnual(TradeableItems eItem);
	DllExport static bool isDual(TradeableItems eItem, bool bExcludePeace = false);
	DllExport static bool hasData(TradeableItems eItem);
	DllExport static bool isGold(TradeableItems eItem);
	DllExport static bool isEndWar(TradeableItems eItem);
	static bool isVassal(TradeableItems eItem);
	DllExport static TradeableItems getPeaceItem();
	DllExport static TradeableItems getGoldItem();
	DllExport static TradeableItems getGoldPerTurnItem();

/************************************************************************************************/
/* Afforess	                  Start		 07/17/10                                               */
/*                                                                                              */
/* Advanced Diplomacy                                                                           */
/************************************************************************************************/
	static bool isSingleOption(TradeableItems eItem);
	bool isEmbassy();
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

protected:
	//	Translate from save-space to load-space enums in trade deal infos
	bool TranslateTradeDataOnLoad(CvTaggedSaveFormatWrapper& wrapper, TradeData& data);

	bool startTrade(TradeData trade, PlayerTypes eFromPlayer, PlayerTypes eToPlayer);
	void endTrade(TradeData trade, PlayerTypes eFromPlayer, PlayerTypes eToPlayer, bool bTeam);

	void startTeamTrade(TradeableItems eItem, TeamTypes eFromTeam, TeamTypes eToTeam, bool bDual);

	void endTeamTrade(TradeableItems eItem, TeamTypes eFromTeam, TeamTypes eToTeam);

	static bool isVassalTrade(const CLinkList<TradeData>* pFirstList);

	int m_iID;
	int m_iInitialGameTurn;

	PlayerTypes m_eFirstPlayer;
	PlayerTypes m_eSecondPlayer;

	CLinkList<TradeData> m_firstTrades;
	CLinkList<TradeData> m_secondTrades;

};

#endif
