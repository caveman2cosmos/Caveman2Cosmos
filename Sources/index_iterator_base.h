#pragma once

#ifndef index_iterator_base_h__
#define index_iterator_base_h__

class index_iterator_core_access
{
public:
	template <class Facade>
	static typename Facade::value_type* first(Facade& f, const typename Facade::owner_type* owner, int* idx, bool reverse)
	{
		return f.first(owner, idx, reverse);
	}

	template <class Facade>
	static typename Facade::value_type* next(Facade& f, const typename Facade::owner_type* owner, int* idx, bool reverse)
	{
		return f.next(owner, idx, reverse);
	}
};

// Base class for iteration using indexed iteration functions (CvPlayer::firstCity/nextCity etc.)
template < class Derived, class Owner, class Value >
class index_iterator_base : public bst::iterator_facade<Derived, Value*, bst::forward_traversal_tag, Value* const&>
{
public:
	typedef Owner owner_type;
	typedef Value value_type;

	index_iterator_base() : m_owner(NULL), m_reverse(false), m_curr(NULL), m_idx(0) {}
	explicit index_iterator_base(const Owner* owner) : m_owner(owner), m_reverse(false), m_idx(0)
	{
		m_curr = index_iterator_core_access::first(derived(), m_owner, &m_idx, m_reverse);
	}
	struct reverse_tag {};
	explicit index_iterator_base(const Owner* owner, reverse_tag) : m_owner(owner), m_reverse(true), m_idx(0)
	{
		m_curr = index_iterator_core_access::first(derived(), m_owner, &m_idx, m_reverse);
	}

private:
	friend class bst::iterator_core_access;
	void increment() 
	{
		m_curr = index_iterator_core_access::next(derived(), m_owner, &m_idx, m_reverse);
	}
	bool equal(index_iterator_base const& other) const
	{
		return (this->m_owner == other.m_owner && this->m_curr == other.m_curr) || (this->m_curr == NULL && other.m_curr == NULL);
	}
	Value* const& dereference() const { return m_curr; }

	Derived& derived()
	{
		return *static_cast<Derived*>(this);
	}

	Derived const& derived() const
	{
		return *static_cast<Derived const*>(this);
	}

	const Owner* m_owner;
	Value* m_curr;
	bool m_reverse;
	int m_idx;
};

#endif // index_iterator_base_h__
