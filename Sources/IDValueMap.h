#pragma once

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    IDValueMap.h
//
//  PURPOSE: A simple mapping from ID to value as a
//
//------------------------------------------------------------------------------------------------
#ifndef IDVALUEMAP_H
#define IDVALUEMAP_H

#include "CvXMLLoadUtility.h"
#include "CvString.h"

// ValueType will usually be int, only value types that are supported by FDataStreamBase as overloaded read and write will work without template specialization
// The maps are assumed to be small, so a vector of pairs is used
template <class ValueType, ValueType& defaultValue>
class IDValueMap
{
public:
	static uint getIDForString(const char* szType);

	void read(FDataStreamBase* pStream)
	{
		unsigned int iSize = 0;
		pStream->Read(&iSize);
		m_map.resize(iSize);
		for (unsigned int i=0; i<iSize; i++)
		{
			pStream->Read(&(m_map[i].first));
			pStream->Read(&(m_map[i].second));
		}
	}

	void write(FDataStreamBase* pStream)
	{
		pStream->Write(m_map.size());
		for (unsigned int i=0; i<m_map.size(); i++)
		{
			pStream->Write(m_map[i].first);
			pStream->Write(m_map[i].second);
		}
	}

	// Call this method with the XML set to the parent node
	void read(CvXMLLoadUtility* pXML)
	{
		CvString szTextVal;
		m_map.clear();
		if (pXML->TryMoveToXmlFirstChild())
		{
			do
			{
				if (pXML->TryMoveToXmlFirstChild())
				{
					pXML->GetXmlVal(szTextVal);
					uint iID = getIDForString(szTextVal);
					ValueType val = defaultValue;
					pXML->GetNextXmlVal(&val);
					setValue(iID, val);
					
					pXML->MoveToXmlParent();
				}
			}
			while (pXML->TryMoveToXmlNextSibling());
			pXML->MoveToXmlParent();
		}
	}

	void copyNonDefaults(IDValueMap<ValueType, defaultValue>* pMap, CvXMLLoadUtility* pXML)
	{
		for (unsigned int i=0; i<pMap->m_map.size(); i++)
		{
			bool bNotFound = true;
			int iID = pMap->m_map[i].first;
			for (unsigned int j=0; j<m_map.size(); j++)
			{
				if (iID == m_map[j].first)
				{
					bNotFound = false;
					break;
				}
			}
			if (bNotFound)
			{
				m_map.push_back(std::make_pair(iID, pMap->m_map[i].second));
			}
		}
	}

	void getCheckSum(unsigned int& iSum)
	{
		for (unsigned int i=0; i<m_map.size(); i++)
		{
			CheckSum(iSum, m_map[i].first);
			CheckSum(iSum, m_map[i].second);
		}
	}

	ValueType getValue(uint iID) const
	{
		for (uint i = 0; i < m_map.size(); i++)
			if (m_map[i].first == iID)
				return m_map[i].second;
		return defaultValue;
	}

	void setValue(uint iID, ValueType val)
	{
		for (uint i = 0; i < m_map.size(); i++)
		{
			if (m_map[i].first == iID)
			{
				m_map[i].second = val;
				return;
			}
		}
		m_map.push_back(std::make_pair(iID, val));
	}


protected:
	std::vector<std::pair<uint,ValueType> > m_map;
	static std::vector<std::pair<uint,CvString> > s_IDmap;
};

extern int g_iPercentDefault;
extern int g_iModifierDefault;

typedef IDValueMap<int, g_iPercentDefault> IDValueMapPercent;
typedef IDValueMap<int, g_iModifierDefault> IDValueMapModifier;

template <class ValueType, ValueType& defaultValue>
std::vector<std::pair<uint,CvString> > IDValueMap<ValueType, defaultValue>::s_IDmap;

template <class ValueType, ValueType& defaultValue>
uint IDValueMap<ValueType, defaultValue>::getIDForString(const char* szType)
{
	for (std::vector<std::pair<uint,CvString> >::iterator it = s_IDmap.begin(); it != s_IDmap.end(); it++)
		if (it->second.CompareNoCase(szType) == 0)
			return it->first;
	uint uiID = s_IDmap.size();
	s_IDmap.push_back(std::make_pair(uiID, szType));
	return uiID;
}

#endif
