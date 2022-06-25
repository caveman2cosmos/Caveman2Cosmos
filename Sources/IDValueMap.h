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

template <class ID_, class Value_, int defaultValue = 0>
struct IDValueMap
{
	typedef std::pair<ID_, Value_>																	value_type;
	typedef typename std::vector<value_type>::iterator												iterator;
	typedef typename std::vector<value_type>::const_iterator										const_iterator;
	typedef CyIterator<iterator, CovertToTuple>														python_iterator;
	typedef bst::filtered_range<bst::function<bool(typename const value_type&)>, const IDValueMap>	filtered;

	iterator begin()	{ return m_map.begin(); }
	iterator end()		{ return m_map.end(); }

	const_iterator begin() const	{ return m_map.begin(); }
	const_iterator end() const		{ return m_map.end(); }

	python_iterator* pyIter()
	{
		return new python_iterator(begin(), end());
	}

	void readPairedArrays(CvXMLLoadUtility* pXML, const wchar_t* szRootTagName, const wchar_t* firstChildTag, const wchar_t* secondChildTag)
	{
		_readPairedArrays<NO_DELAYED_RESOLUTION>(pXML, szRootTagName, firstChildTag, secondChildTag);
	}

	template <DelayedResolutionTypes delayedRes_>
	void _readPairedArrays(CvXMLLoadUtility* pXML, const wchar_t* szRootTagName, const wchar_t* firstChildTag, const wchar_t* secondChildTag)
	{
		FAssert(m_map.empty());

		if (pXML->TryMoveToXmlFirstChild(szRootTagName))
		{
			const int iNumSibs = pXML->GetXmlChildrenNumber();

			if (pXML->TryMoveToXmlFirstChild())
			{
				m_map.resize(iNumSibs); // Important to keep the delayed resolution pointers correct

				foreach_(value_type& pair, m_map)
				{
					CvXMLLoadUtility::SetOptionalInfoType<delayedRes_>(pXML, pair.first, firstChildTag);
					pXML->set(pair.second, secondChildTag);

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
						Value_ value = defaultValue;
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
				m_map.resize(iNumSibs); // Important to keep the delayed resolution pointers correct

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
				m_map.push_back(value_type(otherPair));
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
				m_map.push_back(otherPair);
			}
		}
	}

	void copyNonDefaultDelayedResolution(const IDValueMap& other)
	{
		if (m_map.empty())
		{
			const std::vector<value_type>& otherVector = other.m_map;
			const int num = otherVector.size();
			m_map.resize(num); // Important to keep the delayed resolution pointers correct
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

	bool hasValue(ID_ id) const
	{
		return algo::any_of_equal(m_map | map_keys, id);
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
	template <typename T>
	struct DefaultValue
	{
		static T create(const T& value)
		{
			return value;
		}
	};

	template <typename T, size_t ArraySize>
	struct DefaultValue<bst::array<T, ArraySize> >
	{
		static bst::array<T, ArraySize> create(const T& value)
		{
			bst::array<T, ArraySize> a;
			a.fill(value);
			return a;
		}
	};

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


typedef std::pair<BonusTypes, int> BonusModifier;
typedef std::pair<BuildingTypes, int> BuildingModifier2;
typedef std::pair<BuildTypes, int> BuildModifier2;
typedef std::pair<DomainTypes, int> DomainModifier2;
typedef std::pair<SpecialBuildingTypes, int> SpecialBuildingModifier;
typedef std::pair<ImprovementTypes, int> ImprovementModifier;
typedef std::pair<ReligionTypes, int> ReligionModifier;
typedef std::pair<TechTypes, int> TechModifier;
typedef std::pair<UnitTypes, int> UnitModifier2;
typedef std::pair<UnitCombatTypes, int> UnitCombatModifier2;

typedef std::pair<BuildingTypes, CommerceArray> BuildingCommerce;
typedef std::pair<TechTypes, CommerceArray> TechCommerceArray;
typedef std::pair<TechTypes, YieldArray> TechArray;
typedef std::pair<TerrainTypes, YieldArray> TerrainArray;
typedef std::pair<PlotTypes, YieldArray> PlotArray;

typedef IDValueMap<int, int, 100> IDValueMapPercent;
typedef IDValueMap<int, int, 0> IDValueMapModifier;

#endif