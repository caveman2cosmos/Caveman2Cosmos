//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildLists.cpp
//
//  PURPOSE: Store build lists for a player
//
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvBuildingInfo.h"
#include "CvBuildLists.h"
#include "CvInitCore.h"
#include "CvGlobals.h"
#include <iostream>

CvBuildLists::~CvBuildLists()
{
	for (int i=0; i<(int)m_Lists.size(); i++)
	{
		delete m_Lists[i];
	}
}

int CvBuildLists::getCurrentList() const
{
	return m_iCurrentList;
}

void CvBuildLists::setCurrentList(int i)
{
	m_iCurrentList = i;
}

void CvBuildLists::clearQueue(int iID)
{
	const int index = getIndexByID(iID);
	if (index != -1)
		m_Lists[index]->orderQueue.clear();
}

void CvBuildLists::pushOrder(int iID, OrderTypes eOrder, int iData1, int iData2, bool bSave, bool bPop, bool bAppend)
{
	const int index = getIndexByID(iID);
	if (index == -1)
		return;

	BuildList* pList = m_Lists[index];

	if (bPop)
	{
		popOrder(iID, 0);
	}

	OrderData order;
	order.eOrderType = eOrder;
	order.iData1 = iData1;
	order.iData2 = iData2;
	order.bSave = bSave;

	if (bAppend)
	{
		pList->orderQueue.insertAtEnd(order);
	}
	else
	{
		pList->orderQueue.insertAtBeginning(order);
	}
}

void CvBuildLists::popOrder(int iID, int iNum)
{
	const int index = getIndexByID(iID);
	if (index == -1)
		return;

	BuildList* pList = m_Lists[index];

	if (iNum == -1)
	{
		iNum = (pList->orderQueue.getLength() - 1);
	}

	int iCount = 0;

	CLLNode<OrderData>* pOrderNode = pList->orderQueue.head();

	while (pOrderNode != NULL)
	{
		if (iCount == iNum)
		{
			break;
		}

		iCount++;

		pOrderNode = pList->orderQueue.next(pOrderNode);
	}

	if (pOrderNode == NULL)
	{
		return;
	}

	pList->orderQueue.deleteNode(pOrderNode);
}

int CvBuildLists::getNumLists() const
{
	return (int)m_Lists.size();
}

int CvBuildLists::getIndexByID(int iID) const
{
	for (int i=0; i<(int)m_Lists.size(); i++)
		if (m_Lists[i]->iID == iID)
			return i;
	return -1;
}

int CvBuildLists::getID(int index) const
{
	FASSERT_BOUNDS(0, getNumLists(), index);
	return m_Lists[index]->iID;
}

const CvString CvBuildLists::getListName(int index) const
{
	FASSERT_BOUNDS(0, getNumLists(), index);
	return m_Lists[index]->szName;
}

int CvBuildLists::getListLength(int index) const
{
	FASSERT_BOUNDS(0, getNumLists(), index);
	return m_Lists[index]->orderQueue.getLength();
}

const OrderData* CvBuildLists::getOrder(int index, int iQIndex) const
{
	FASSERT_BOUNDS(0, getNumLists(), index);
	const CLLNode<OrderData>* pOrderNode = m_Lists[index]->orderQueue.nodeNum(iQIndex);
	if (pOrderNode)
	{
		return &(pOrderNode->m_data);
	}
	return NULL;
}

int CvBuildLists::addList(const CvString szName, int iID)
{
	if (iID == -1)
	{
		m_iMaxID++;
		iID = m_iMaxID;
	}
	else
	{
		if (iID > m_iMaxID)
		{
			m_iMaxID = iID;
		}
	}
	const int iLast = (int)m_Lists.size();
	m_Lists.push_back(new BuildList());
	m_Lists[iLast]->iID = iID;
	m_Lists[iLast]->szName = szName;
	m_iCurrentList = m_iMaxID;
	return m_iMaxID;
}

void CvBuildLists::removeList(int iID)
{
	for (std::vector<BuildList*>::iterator it = m_Lists.begin(); it != m_Lists.end(); ++it)
	{
		if ((*it)->iID == iID)
		{
			delete *it;
			m_Lists.erase(it);
			// Update current list
			if (m_iCurrentList == iID)
			{
				if (!m_Lists.empty())
				{
					m_iCurrentList = getID(0);
				}
				else
				{
					m_iCurrentList = -1;
				}
			}
			return;
		}
	}
}

void CvBuildLists::renameList(int iID, const CvString szName)
{
	const int index = getIndexByID(iID);
	if (index > -1)
	{
		m_Lists[index]->szName = szName;
	}
}

void CvBuildLists::readFromFile()
{
	const CvString szFile = GC.getInitCore().getDLLPath() + "\\..\\UserSettings\\BuildLists.txt";
	std::ifstream stream;
	stream.open(szFile.c_str());
	if (!stream.is_open())
		return;

	char szLine[1024];
	while (stream.good() && !stream.eof())
	{
		stream.getline(szLine, 1024);
		if (strlen(szLine) < 3)
			continue;

		if (szLine[0] == 'l')
		{
			int iID = -1;
			stream >> iID;
			addList(szLine+2, iID);
		}
		else if ((szLine[0] == 'u') || (szLine[0] == 'b') || (szLine[0] == 'p'))
		{
			int index = getNumLists() - 1;
			if (index < 0)
			{
				addList("");
				index = 0;
			}

			OrderTypes eOrder = NO_ORDER;
			if (szLine[0] == 'u')
				eOrder = ORDER_TRAIN;
			else if (szLine[0] == 'b')
				eOrder = ORDER_CONSTRUCT;
			else
				eOrder = ORDER_CREATE;

			const bool bSave = szLine[1] == '*';
			const int iData1 = GC.getInfoTypeForString(szLine+2);
			pushOrder(getID(index), eOrder, iData1, 0, bSave, false, true);
		}
	}
	stream.close();
}

void CvBuildLists::writeToFile()
{
	const CvString szFile = GC.getInitCore().getDLLPath() + "\\..\\UserSettings\\BuildLists.txt";
	std::ofstream stream;
	stream.open(szFile.c_str(), std::ios::trunc);
	if (!stream.is_open())
		return;

	for (int iList=0; iList < getNumLists(); iList++)
	{
		stream << "l:" << getListName(iList).c_str() << std::endl << getID(iList) << std::endl;

		for (int i=0; i < getListLength(iList); i++)
		{
			CvString szType;
			const OrderData* pOrder = getOrder(iList, i);
			if (pOrder->eOrderType == ORDER_TRAIN)
			{
				stream << 'u';
				szType = GC.getUnitInfo((UnitTypes)pOrder->iData1).getType();
			}
			else if (pOrder->eOrderType == ORDER_CONSTRUCT)
			{
				stream << 'b';
				szType = GC.getBuildingInfo((BuildingTypes)pOrder->iData1).getType();
			}
			else
			{
				stream << 'p';
				szType = GC.getProjectInfo((ProjectTypes)pOrder->iData1).getType();
			}

			if (pOrder->bSave)
				stream << '*';
			else
				stream << ':';

			stream << szType.c_str() << std::endl;
		}

		stream << std::endl;
	}
	stream.close();
}

void CvBuildLists::read(FDataStreamBase *pStream)
{
	int iNum = 0;
	pStream->Read(&m_iMaxID);
	pStream->Read(&iNum);
	m_Lists.resize(iNum);
	for (int i=0; i<iNum; i++)
	{
		m_Lists[i] = new BuildList();
		pStream->Read(&(m_Lists[i]->iID));
		pStream->ReadString(m_Lists[i]->szName);
		m_Lists[i]->orderQueue.ReadNonWrapper(pStream);
	}
	// Update current list
	if (iNum > 0)
	{
		m_iCurrentList = getID(0);
	}
	else
	{
		m_iCurrentList = -1;
	}
}

void CvBuildLists::write(FDataStreamBase *pStream)
{
	pStream->Write(m_iMaxID);
	const int iNum = getNumLists();
	pStream->Write(iNum);
	for (int i=0; i<iNum; i++)
	{
		pStream->Write(m_Lists[i]->iID);
		pStream->WriteString(m_Lists[i]->szName);
		m_Lists[i]->orderQueue.WriteNonWrapper(pStream);
	}
}

void CvBuildLists::readSubset(FDataStreamBase *pStream)
{
	int iList = 0;
	pStream->Read(&m_iMaxID);
	pStream->Read(&iList);

	// make new lists if necessary
	for (int i=getNumLists(); i<=iList; i++)
	{
		m_Lists.push_back(new BuildList());
	}

	pStream->Read(&(m_Lists[iList]->iID));
	pStream->ReadString(m_Lists[iList]->szName);
	m_Lists[iList]->orderQueue.ReadNonWrapperSubset(pStream);

	// Update current list
	if (iList >= 0)
	{
		m_iCurrentList = getID(0);
	}
	else
	{
		m_iCurrentList = -1;
	}
}

void CvBuildLists::writeSubset(FDataStreamBase *pStream, int iList, int index, int iNum)
{
	if (iList >= getNumLists())
		return;
	pStream->Write(m_iMaxID);
	pStream->Write(iList);
	pStream->Write(m_Lists[iList]->iID);
	pStream->WriteString(m_Lists[iList]->szName);
	m_Lists[iList]->orderQueue.WriteNonWrapperSubset(pStream, index, iNum);
}