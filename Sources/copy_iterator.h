#pragma once

#ifndef copy_iterator_h__
#define copy_iterator_h__

template <class Value, class Ref>
class copy_iterator : public boost::iterator_facade<copy_iterator<Value, Ref>, Value, boost::forward_traversal_tag, Ref>
{
public:
	copy_iterator() : m_idx(-1) {}
	template < class OtherItr_ >
	explicit copy_iterator(OtherItr_ begin, OtherItr_ end)
		: m_copy(begin, end)
		, m_idx(-1)
	{
		if (m_copy.size() > 0)
			m_idx = 0;
	}

private:
	friend class boost::iterator_core_access;

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

	Value dereference() const { return m_copy[m_idx]; }

	std::vector<Value> m_copy;
	int m_idx;
};
#endif // copy_iterator_h__