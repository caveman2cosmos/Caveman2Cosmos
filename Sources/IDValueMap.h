#pragma once

#ifndef IDVALUEMAP_H
#define IDVALUEMAP_H

//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:    IDValueMap.h
//
//  PURPOSE: A simple mapping from ID to value as a
//
//------------------------------------------------------------------------------------------------

#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"

// The maps are assumed to be small, so a vector of pairs is used

template <class ID_, class Value_, Value_ defaultValue = static_cast<Value_>(0)>
struct IDValueMap
{
/*
	template <class ID_, class Value_>
	struct pair2
	{
		pair(ID_ otherID, Value_ otherValue)
			: type(otherID)
			, value(otherValue)
		{}

		const ID_ type;
		const Value_ value;
	};
*/
	typedef std::pair<ID_, Value_> pair_t;

	void read(CvXMLLoadUtility* pXML, const wchar_t* szRootTagName)
	{
		if (pXML->TryMoveToXmlFirstChild(szRootTagName))
		{
			m_map.clear();
			if (pXML->TryMoveToXmlFirstChild())
			{
				do
				{
					if (pXML->TryMoveToXmlFirstChild())
					{
						CvString szTextVal;
						pXML->GetXmlVal(szTextVal);
						const ID_ type = (ID_)GC.getOrCreateInfoTypeForString(szTextVal);
						Value_ value = defaultValue;
						pXML->GetNextXmlVal(&value);
						m_map.push_back(std::make_pair(type, value));
						pXML->MoveToXmlParent();
					}
				} while (pXML->TryMoveToXmlNextSibling());
				pXML->MoveToXmlParent();
			}
			pXML->MoveToXmlParent();
		}
	}

	void readWithDelayedResolution(CvXMLLoadUtility* pXML, const wchar_t* szRootTagName)
	{
		if (pXML->TryMoveToXmlFirstChild(szRootTagName))
		{
			m_map.clear();
			const int iNumSibs = pXML->GetXmlChildrenNumber();
			m_map.resize(iNumSibs);

			if (iNumSibs > 0)
			{
				if (pXML->TryMoveToXmlFirstChild())
				{
					CvString szTextVal;
					foreach_(pair_t& pair, m_map)
					{
						if (pXML->GetChildXmlVal(szTextVal))
						{
							Value_ modifier = defaultValue;
							pXML->GetNextXmlVal(&modifier);
							pair = std::make_pair((ID_)-1, modifier);
							GC.addDelayedResolution((int*)&pair.first, szTextVal);
							pXML->MoveToXmlParent();
						}
						if (!pXML->TryMoveToXmlNextSibling())
						{
							break;
						}
					}
					pXML->MoveToXmlParent();
				}
			}
			pXML->MoveToXmlParent();
		}
	}

	void copyNonDefaults(const IDValueMap<ID_, Value_, defaultValue>& other)
	{
		foreach_(const pair_t& otherPair, other)
		{
			if (!hasValue(otherPair.first))
			{
				m_map.push_back(std::make_pair(otherPair.first, otherPair.second));
			}
		}
	}

	void copyNonDefaultDelayedResolution(const IDValueMap<ID_, Value_, defaultValue>& other)
	{
		if (m_map.empty())
		{
			const std::vector<std::pair<ID_, Value_> >& otherVector = other.m_map;
			const int num = otherVector.size();
			m_map.resize(num);
			for (int i = 0; i < num; i++)
			{
				m_map[i] = std::make_pair(static_cast<ID_>(-1), otherVector[i].second);
				GC.copyNonDefaultDelayedResolution((int*)&m_map[i].first, (int*)&otherVector[i].first);
			}
		}
	}

	void removeDelayedResolution()
	{
		foreach_(const pair_t& pair, m_map)
			GC.removeDelayedResolution((int*)&pair.first);
	}

	void getCheckSum(uint32_t& iSum) const
	{
		foreach_(const pair_t& pair, m_map)
		{
			CheckSum(iSum, pair.first);
			CheckSum(iSum, pair.second);
		}
	}

	Value_ getValue(ID_ id) const
	{
		foreach_(const pair_t& pair, m_map)
			if (pair.first == id)
				return pair.second;
		return defaultValue;
	}

	bool hasValue(ID_ type) const
	{
		foreach_(const pair_t& pair, m_map)
			if (pair.first == type)
				return true;
		return false;
	}

	int size() const
	{
		return (int)m_map.size();
	}

	//const pair_t& operator[](int i) const
	//{
	//	FASSERT_BOUNDS(0, size(), i)
	//	return m_map[i];
	//}

	//operator std::vector<pair_t>() const { return m_map; }

	typedef typename std::vector<pair_t>::iterator        iterator;
	typedef typename std::vector<pair_t>::const_iterator  const_iterator;

	iterator begin() { return m_map.begin(); }
	iterator end()   { return m_map.end(); }

	const_iterator begin() const { return m_map.begin(); }
	const_iterator end() const   { return m_map.end(); }

protected:
	std::vector<pair_t> m_map;
};

typedef std::pair<TechTypes, int> TechModifier;

typedef IDValueMap<int, int, 100> IDValueMapPercent;
typedef IDValueMap<int, int, 0> IDValueMapModifier;

#endif
