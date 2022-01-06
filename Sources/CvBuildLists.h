#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvBuildLists.h
//
//  PURPOSE: Store build lists for a player
//
//------------------------------------------------------------------------------------------------

#ifndef CV_BUILD_LISTS_H
#define CV_BUILD_LISTS_H

//#include "CvStructs.h"

struct BuildList
{
	int iID;
	CvString szName;
	CLinkList<OrderData> orderQueue;
};

class CvBuildLists
{
public:
	CvBuildLists() : m_iMaxID(0), m_iCurrentList(-1) {};
	~CvBuildLists();

	// current list is async, rest is synced
	int getCurrentList() const;
	void setCurrentList(int i);

	void clearQueue(int iID);
	void pushOrder(int iID, OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend);
	void popOrder(int iID, int iNum = -1);

	int getNumLists() const;
	int getIndexByID(int iID) const;
	int getID(int index) const;
	const CvString getListName(int index) const;
	int getListLength(int index) const;
	const OrderData* getOrder(int index, int iQIndex) const;

	int addList(const CvString szName, int iID = -1);
	void removeList(int iID);
	void renameList(int iID, const CvString szName);

	void readFromFile();
	void writeToFile();

	void read( FDataStreamBase* pStream );
	void write( FDataStreamBase* pStream );

	void readSubset( FDataStreamBase* pStream );
	void writeSubset( FDataStreamBase* pStream, int iList, int index, int iNum );

protected:
	int m_iMaxID;
	int m_iCurrentList;
	std::vector<BuildList*> m_Lists;
};


#endif
