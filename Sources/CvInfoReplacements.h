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

//#include "BoolExpr.h"
//#include "CvGameAI.h"
//#include "CvGlobals.h"
//#include "CvInfos.h"

template <class T>
class CvInfoReplacement
{
protected:
	const uint m_uiID;
	const uint m_uiReplID;
	const BoolExpr* m_pCondition;
	T* m_pInfo;

public:
	CvInfoReplacement(uint uiID = 0, uint uiReplID = 0, const BoolExpr* pCondition = NULL, T* pInfo = NULL):
		m_uiID(uiID), m_uiReplID(uiReplID), m_pCondition(pCondition), m_pInfo(pInfo) {}

	void read(FDataStreamBase* pStream)
	{
		pStream->Read(&m_uiID);
		pStream->Read(&m_uiReplID);
		m_pCondition = BoolExpr::readExpression(pStream);
		m_pInfo = new T();
		((CvInfoBase*)m_pInfo)->read(pStream);
	}

	void write(FDataStreamBase* pStream)
	{
		pStream->Write(m_uiID);
		pStream->Write(m_uiReplID);
		m_pCondition->write(pStream);
		((CvInfoBase*)m_pInfo)->write(pStream);
	}

	uint getID() const
	{
		return m_uiID;
	}

	uint getReplacementID() const
	{
		return m_uiReplID;
	}

	bool checkCondition()
	{
		return m_pCondition->evaluate(GC.getGame().getGameObject());
	}

	T* getInfo() const
	{
		return m_pInfo;
	}

	void setInfo(T* pInfo)
	{
		m_pInfo = pInfo;
	}

//	void updateInfo(const BoolExpr* pCondition, T* pInfo)
//	{
//		m_pCondition = pCondition;
//		pInfo->copyNonDefaults(m_pInfo, NULL);
//		SAFE_DELETE(m_pInfo);
//		m_pInfo = pInfo;
//	}
};

template <class T>
class CvInfoReplacements
{
protected:
	std::vector<CvInfoReplacement<T>* > m_apReplacements;
	std::vector<std::pair<int, T*> > m_apBackups;
	static std::vector<std::pair<uint,CvString> > s_IDmap;

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
		const unsigned int iSize = m_apReplacements.size();
		pStream->Write(iSize);
		for (unsigned int i=0; i<iSize; i++)
		{
			m_apReplacements[i]->write(pStream);
		}
	}

	static uint getReplacementIDForString(const char* szType);

	CvInfoReplacement<T>* getReplacement(uint uiID, uint uiReplID) const
	{
		const unsigned int iSize = m_apReplacements.size();
		for (unsigned int i=0; i<iSize; i++)
		{
			CvInfoReplacement<T>* pReplacement = m_apReplacements[i];
			if ((pReplacement->getID() == uiID) && (pReplacement->getReplacementID() == uiReplID))
				return pReplacement;
		}
		return NULL;
	}

	// This adds a replacement or updates an existing one
	void addReplacement(uint uiID, uint uiReplID, const BoolExpr* pCondition, T* pInfo/*, bool bPassTwo = false*/)
	{
//		CvInfoReplacement<T>* pExisting = getReplacement(iID, iReplID);
//		if (pExisting)
//		{
//			pExisting->updateInfo(pCondition, pInfo, bPassTwo);
//		}
//		else
//		{
			m_apReplacements.push_back(new CvInfoReplacement<T>(uiID, uiReplID, pCondition, pInfo));
//		}
	}

	T* getReplacementInfo(uint uiID, uint uiReplID) const
	{
		const CvInfoReplacement<T>* pReplacement = getReplacement(uiID, uiReplID);
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
			const int iID = pReplacement->getID();

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

template <class T>
std::vector<std::pair<uint,CvString> > CvInfoReplacements<T>::s_IDmap;

template <class T>
uint CvInfoReplacements<T>::getReplacementIDForString(const char* szType)
{
	for (std::vector<std::pair<uint,CvString> >::iterator it = s_IDmap.begin(); it != s_IDmap.end(); ++it)
	{
		if (it->second.CompareNoCase(szType) == 0)
		{
			return it->first;
		}
	}
	uint uiID = s_IDmap.size();
	s_IDmap.push_back(std::make_pair(uiID, szType));
	return uiID;
}

#endif