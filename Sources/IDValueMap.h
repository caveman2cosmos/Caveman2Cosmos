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

class CvXMLLoadUtility;

// The maps are assumed to be small, so a vector of pairs is used
template <class ValueType, ValueType& defaultValue>
class IDValueMap
{
public:
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
					int iID = GC.getOrCreateInfoTypeForString(szTextVal);
					ValueType val = defaultValue;
					pXML->GetNextXmlVal(&val);
					setValue(iID, val);

					pXML->MoveToXmlParent();
				}
			} while (pXML->TryMoveToXmlNextSibling());
			pXML->MoveToXmlParent();
		}
	}

	void copyNonDefaults(IDValueMap<ValueType, defaultValue>* pMap, CvXMLLoadUtility* pXML)
	{
		for (unsigned int i = 0; i < pMap->m_map.size(); i++)
		{
			bool bNotFound = true;
			int iID = pMap->m_map[i].first;
			for (unsigned int j = 0; j < m_map.size(); j++)
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

	void getCheckSum(unsigned int& iSum) const
	{
		for (unsigned int i = 0; i < m_map.size(); i++)
		{
			CheckSum(iSum, m_map[i].first);
			CheckSum(iSum, m_map[i].second);
		}
	}

	ValueType getValue(int iID) const
	{
		for (unsigned int i = 0; i < m_map.size(); i++)
			if (m_map[i].first == iID)
				return m_map[i].second;
		return defaultValue;
	}

	void setValue(int iID, ValueType val)
	{
		for (unsigned int i = 0; i < m_map.size(); i++)
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
	std::vector<std::pair<int, ValueType> > m_map;
};

extern int g_iPercentDefault;
extern int g_iModifierDefault;

typedef IDValueMap<int, g_iPercentDefault> IDValueMapPercent;
typedef IDValueMap<int, g_iModifierDefault> IDValueMapModifier;

#endif
