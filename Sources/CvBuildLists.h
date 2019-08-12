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

#include "CvStructs.h"

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
	int getCurrentList();
	void setCurrentList(int i);
	
	void clearQueue(int iID);
	void pushOrder(int iID, OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend);
	void popOrder(int iID, int iNum = -1);

	int getNumLists();
	int getIndexByID(int iID);
	int getID(int index);
	CvString getListName(int index);
	int getListLength(int index);
	OrderData* getOrder(int index, int iQIndex);

	int addList(CvString szName, int iID = -1);
	void removeList(int iID);
	void renameList(int iID, CvString szName);
	
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
