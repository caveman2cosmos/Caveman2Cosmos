#pragma once

#ifndef CY_DEAL_H
#define CY_DEAL_H

//
// Python wrapper class for CvDeal
//

class CvDeal;

class CyDeal
{
public:
	CyDeal(CvDeal* pDeal = NULL);
	virtual ~CyDeal();

	CvDeal* getDeal() const { return m_pDeal; }
	bool isNone() const { return m_pDeal == NULL;  }

	int getID() const;
	int getInitialGameTurn() const;
	int getFirstPlayer() const;
	int getSecondPlayer() const;
	int getLengthFirstTrades() const;
	int getLengthSecondTrades() const;
	TradeData* getFirstTrade(int i) const;
	TradeData* getSecondTrade(int i) const;

	void kill();

	bool isCancelable(int /*PlayerTypes*/ eByPlayer, bool bIgnoreWaitingPeriod = false) const;
	std::wstring getCannotCancelReason(int /*PlayerTypes*/ eByPlayer) const;
	int turnsToCancel(int /*PlayerTypes*/ eByPlayer) const;

protected:
	CvDeal* m_pDeal;
};

#endif
