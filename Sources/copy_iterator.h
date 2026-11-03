#pragma once

#ifndef copy_iterator_h__
#define copy_iterator_h__

#include "FProfiler.h"
#include "FAssert.h"

template <class Value>
class copy_iterator : public bst::iterator_facade<copy_iterator<Value>, Value*, bst::forward_traversal_tag, Value*>
{
public:
	copy_iterator() : m_idx(-1) {}
	template < class OtherItr_ >
	explicit copy_iterator(OtherItr_ begin, OtherItr_ end)
		: m_idx(-1)
	{
		PROFILE_EXTRA_FUNC();
		// Safety net: the source list (eg. a CvSelectionGroup's unit linked list) has, on rare
		// occasions, been observed corrupted into a cycle, which would otherwise spin here
		// forever (begin never reaching end) and hang the whole game. No legitimate list this
		// gets used on is anywhere near this size, so hitting the cap means corruption, not a
		// false positive; better to stop with a partial, safe copy than hang indefinitely.
		static const int MAX_COPY_SAFETY_CAP = 100000;
		for (; begin != end; ++begin)
		{
			if (static_cast<int>(m_copy.size()) >= MAX_COPY_SAFETY_CAP)
			{
				FErrorMsg("copy_iterator: safety cap hit while copying source range - likely a corrupted/cyclic underlying list, aborting copy early");
				break;
			}
			m_copy.push_back(*begin);
		}

		if (m_copy.size() > 0)
		{
			m_idx = 0;
		}
	}

private:
	friend class bst::iterator_core_access;

	void increment()
	{
		if(m_idx != -1)
		{
			m_idx = m_idx + 1;
			if (m_idx >= static_cast<int>(m_copy.size()))
				m_idx = -1;
		}
	}

	bool equal(copy_iterator const& other) const
	{
		return this->m_idx == other.m_idx;
	}

	Value* dereference() const { return m_copy[m_idx]; }

	std::vector<Value*> m_copy;
	int m_idx;
};

class CvUnit;

// Safe city and unit iterators (they copy the whole range before iterating, but this is just copying pointers so not a big deal in most cases
// However it shouldn't be used in inner loops
typedef copy_iterator<CvCity> safe_city_iterator;
typedef copy_iterator<CvUnit> safe_unit_iterator;

#endif // copy_iterator_h__