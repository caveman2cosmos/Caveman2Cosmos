#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    CvInfoReplacements.h
//
//  PURPOSE: Handling of conditional replacements for Civ4 info classes
//
//------------------------------------------------------------------------------------------------
#ifndef CVINFOREPLACEMENTS_H
#define CVINFOREPLACEMENTS_H

class BoolExpr;
class cvInternalGlobals;
class CvInfoBase;

#include <set>

template <class T>
class CvInfoReplacement
{
protected:
	int m_iID;
	int m_iReplID;
	BoolExpr* m_pCondition;
	T* m_pInfo;

public:
	CvInfoReplacement(int iID = 0, int iReplID = 0, BoolExpr* pCondition = NULL, T* pInfo = NULL) : m_iID(iID), m_iReplID(iReplID), m_pCondition(pCondition), m_pInfo(pInfo) {}

	void read(FDataStreamBase* pStream)
	{
		pStream->Read(&m_iID);
		pStream->Read(&m_iReplID);
		m_pCondition = BoolExpr::readExpression(pStream);
		m_pInfo = new T();
		((CvInfoBase*)m_pInfo)->read(pStream);
	}

	void write(FDataStreamBase* pStream)
	{
		pStream->Write(m_iID);
		pStream->Write(m_iReplID);
		m_pCondition->write(pStream);
		((CvInfoBase*)m_pInfo)->write(pStream);
	}

	int getID()
	{
		return m_iID;
	}

	int getReplacementID()
	{
		return m_iReplID;
	}

	bool checkCondition()
	{
		return m_pCondition->evaluate(cvInternalGlobals::getInstance().getGameINLINE().getGameObject());
	}

	T* getInfo()
	{
		return m_pInfo;
	}

	void setInfo(T* pInfo)
	{
		m_pInfo = pInfo;
	}

//	void updateInfo(BoolExpr* pCondition, T* pInfo, bool bPassTwo = false)
//	{
//		if (bPassTwo)
//		{
//			SAFE_DELETE(pCondition);
//			m_pInfo->copyNonDefaultsReadPass2(pInfo, NULL);
//			SAFE_DELETE(pInfo);
//		}
//		else
//		{
//			m_pCondition = pCondition;
//			pInfo->copyNonDefaults(m_pInfo, NULL);
//			SAFE_DELETE(m_pInfo);
//			m_pInfo = pInfo;
//		}
//	}
};

template <class T>
class CvInfoReplacements
{
protected:
	std::vector<CvInfoReplacement<T>* > m_apReplacements;
	std::vector<std::pair<int, T*> > m_apBackups;

public:
	void read(FDataStreamBase* pStream)
	{
		unsigned int iSize;
		pStream->Read(&iSize);
		for (unsigned int i=0; i<iSize; i++)
		{
			CvInfoReplacement<T>* pReplacement = new CvInfoReplacement<T>();
			pReplacement->read(pStream);
			m_apReplacements.push_back(pReplacement);
		}
	}

	void write(FDataStreamBase* pStream)
	{
		unsigned int iSize = m_apReplacements.size();
		pStream->Write(iSize);
		for (unsigned int i=0; i<iSize; i++)
		{
			m_apReplacements[i]->write(pStream);
		}
	}

	CvInfoReplacement<T>* getReplacement(int iID, int iReplID)
	{
		unsigned int iSize = m_apReplacements.size();
		for (unsigned int i=0; i<iSize; i++)
		{
			CvInfoReplacement<T>* pReplacement = m_apReplacements[i];
			if ((pReplacement->getID() == iID) && (pReplacement->getReplacementID() == iReplID))
				return pReplacement;
		}
		return NULL;
	}
	
	// This adds a replacement or updates an existing one
	void addReplacement(int iID, int iReplID, BoolExpr* pCondition, T* pInfo, bool bPassTwo = false)
	{
//		CvInfoReplacement<T>* pExisting = getReplacement(iID, iReplID);
//		if (pExisting)
//		{
//			pExisting->updateInfo(pCondition, pInfo, bPassTwo);
//		}
//		else
//		{
			m_apReplacements.push_back(new CvInfoReplacement<T>(iID, iReplID, pCondition, pInfo));
//		}
	}

	T* getReplacementInfo(int iID, int iReplID)
	{
		CvInfoReplacement<T>* pReplacement = getReplacement(iID, iReplID);
		if (pReplacement)
		{
			return pReplacement->getInfo();
		}
		return NULL;
	}

	void readPass3()
	{
		for (unsigned int i=0; i<m_apReplacements.size(); i++)
		{
			m_apReplacements[i]->getInfo()->readPass3();
		}
	}

	void updateReplacements(std::vector<T*>& aInfos)
	{
		// Restore base info classes from backup
		unsigned int iSize = m_apBackups.size();
		for (unsigned int i=0; i<iSize; i++)
		{
			aInfos[m_apBackups[i].first] = m_apBackups[i].second;
		}
		m_apBackups.clear();

		// Switch in all replacements for which the condition is true, but only the first right one if there is more than one
		std::set<int> setSwitched;
		
		iSize = m_apReplacements.size();
		for (unsigned int i=0; i<iSize; i++)
		{
			CvInfoReplacement<T>* pReplacement = m_apReplacements[i];
			int iID = pReplacement->getID();

			if (setSwitched.count(iID) == 0)
			{
				if (pReplacement->checkCondition())
				{
					m_apBackups.push_back(std::make_pair(iID, aInfos[iID]));
					aInfos[iID] = pReplacement->getInfo();
					setSwitched.insert(iID);
				}
			}
		}
	}
};

#endif