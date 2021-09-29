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
#include "CyIterator.h"

// The maps are assumed to be small, so a vector of pairs is used

template <class ID_, class Value_, Value_ defaultValue = static_cast<Value_>(0)>
struct IDValueMap
{
	typedef std::pair<ID_, Value_> value_type;

	typedef typename std::vector<value_type>::iterator			iterator;
	typedef typename std::vector<value_type>::const_iterator	const_iterator;

	iterator begin()	{ return m_map.begin(); }
	iterator end()		{ return m_map.end(); }

	const_iterator begin() const	{ return m_map.begin(); }
	const_iterator end() const		{ return m_map.end(); }

	typedef CyIterator<iterator, CovertToTuple> python_iterator;

	python_iterator* pyIter()
	{
		return new python_iterator(begin(), end());
	}

	void readPairedArrays(CvXMLLoadUtility* pXML, const wchar_t* szRootTagName, const wchar_t* firstChildTag, const wchar_t* secondChildTag, int pairedArraySize)
	{
		if (pXML->TryMoveToXmlFirstChild(szRootTagName))
		{
			const int iNumChildren = pXML->GetXmlChildrenNumber();

			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumChildren; ++j)
				{
					CvString szTextVal;
					pXML->GetChildXmlValByName(szTextVal, firstChildTag);
					value_type pair = value_type();
					pair.first = static_cast<ID_>(GC.getInfoTypeForString(szTextVal));
					if (pair.first > -1)
					{
						pXML->SetList(&pair.second, pairedArraySize, secondChildTag);
						m_map.push_back(pair);
					}
					if (!pXML->TryMoveToXmlNextSibling())
						break;
				}
				pXML->MoveToXmlParent();
			}
			pXML->MoveToXmlParent();
		}
	}

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
						int value = defaultValue;
						pXML->GetNextXmlVal(&value);
						m_map.push_back(std::make_pair(static_cast<ID_>(GC.getOrCreateInfoTypeForString(szTextVal)), value));
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

			if (iNumSibs > 0)
			{
				m_map.resize(iNumSibs);

				if (pXML->TryMoveToXmlFirstChild())
				{
					foreach_(value_type& pair, m_map)
					{
						CvString szTextVal;
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

	void copyNonDefaults(const IDValueMap& other)
	{
		foreach_(const value_type& otherPair, other)
		{
			if (!hasValue(otherPair.first))
			{
				m_map.push_back(std::make_pair(otherPair.first, otherPair.second));
			}
		}
	}

#define COPY(dst, src, typeName)						 \
	{													 \
		const int iNum = sizeof(src) / sizeof(typeName); \
		dst = new typeName[iNum];						 \
		for (int i = 0; i < iNum; i++)					 \
			dst[i] = src[i];							 \
	}

	void copyNonDefaultPairedArrays(const IDValueMap& other)
	{
		foreach_(const value_type& otherPair, other)
		{
			if (!hasValue(otherPair.first))
			{
				value_type pair = value_type();
				pair.first = otherPair.first;
				COPY(pair.second, otherPair.second, Value_);
				m_map.push_back(pair);
			}
		}
	}

	void copyNonDefaultDelayedResolution(const IDValueMap& other)
	{
		if (m_map.empty())
		{
			const std::vector<value_type>& otherVector = other.m_map;
			const int num = otherVector.size();
			m_map.resize(num);
			for (int i = 0; i < num; i++)
			{
				m_map[i] = std::make_pair((ID_)-1, otherVector[i].second);
				GC.copyNonDefaultDelayedResolution((int*)&m_map[i].first, (int*)&otherVector[i].first);
			}
		}
	}

	void removeDelayedResolution()
	{
		foreach_(const value_type& pair, m_map)
			GC.removeDelayedResolution((int*)&pair.first);
	}

	Value_ getValue(ID_ id) const
	{
		foreach_(const value_type& pair, m_map)
			if (pair.first == id)
				return pair.second;
		return defaultValue;
	}

	bool hasValue(ID_ type) const
	{
		foreach_(const value_type& pair, m_map)
			if (pair.first == type)
				return true;
		return false;
	}

	bool empty() const
	{
		return m_map.empty();
	}

	const python::list makeList() const
	{
		python::list l = python::list();
		foreach_(const value_type& pair, m_map)
			l.append(std::make_pair((int)pair.first, pair.second));
		return l;
	}

private:
	std::vector<value_type> m_map;
};


template <typename IDValueMap_t>
void publishIDValueMapPythonInterface()
{
	python::class_<IDValueMap_t, boost::noncopyable>("IDValueMap", python::no_init)
		.def("__iter__", &IDValueMap_t::pyIter, python::return_value_policy<python::manage_new_object>())
		.def("__contains__", &IDValueMap_t::hasValue)
		.def("getValue", &IDValueMap_t::getValue)
	;

	publishPythonIteratorInterface<IDValueMap_t::python_iterator>();
}

typedef std::pair<BonusTypes, int> BonusModifier2;
typedef std::pair<BuildingTypes, int> BuildingModifier2;
typedef std::pair<TechTypes, int> TechModifier;
typedef std::pair<UnitTypes, int> UnitModifier2;
typedef std::pair<ReligionTypes, int> ReligionModifier;
typedef std::pair<UnitCombatTypes, int> UnitCombatModifier2;

typedef std::pair<TechTypes, int*> TechArray;
typedef std::pair<TerrainTypes, int*> TerrainYieldChanges;

typedef IDValueMap<int, int, 100> IDValueMapPercent;
typedef IDValueMap<int, int, 0> IDValueMapModifier;

#endif