#pragma once

#ifndef index_iterator_base_h__
#define index_iterator_base_h__

class index_iterator_core_access
{
public:
	template <class Facade>
	static typename Facade::value_type* first(Facade& f, typename Facade::owner_type* owner, int* idx, bool reverse)
	{
		return f.first(owner, idx, reverse);
	}

	template <class Facade>
	static typename Facade::value_type* next(Facade& f, typename Facade::owner_type* owner, int* idx, bool reverse)
	{
		return f.next(owner, idx, reverse);
	}
};

// Base class for iteration using indexed iteration functions (CvPlayer::firstCity/nextCity etc.)
template < class Derived, class Owner, class Value >
class index_iterator_base : public bst::iterator_facade<Derived, Value*, bst::forward_traversal_tag, Value&>
{
public:
	typedef Owner owner_type;
	typedef Value value_type;

	index_iterator_base() : m_owner(NULL), m_reverse(false), m_curr(NULL), m_idx(0) {}
	explicit index_iterator_base(Owner* owner) : m_owner(owner), m_reverse(false), m_idx(0)
	{
		m_curr = index_iterator_core_access::first(derived(), m_owner, &m_idx, m_reverse);
	}
	struct reverse_tag {};
	explicit index_iterator_base(Owner* owner, reverse_tag) : m_owner(owner), m_reverse(true), m_idx(0)
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
	Value& dereference() const { return *m_curr; }

	Derived& derived()
	{
		return *static_cast<Derived*>(this);
	}

	Derived const& derived() const
	{
		return *static_cast<Derived const*>(this);
	}

	Owner* m_owner;
	Value* m_curr;
	bool m_reverse;
	int m_idx;
};

#define DECLARE_INDEX_ITERATORS(owner_type, item_type, base_name, itr_name, const_itr_name, first_fn, next_fn) \
	template < class OwnerTy_, class ItemTy_ > \
	class base_name : public index_iterator_base<base_name<OwnerTy_, ItemTy_>, OwnerTy_, ItemTy_> \
	{ \
	public: \
		typedef index_iterator_base<base_name<OwnerTy_, ItemTy_>, OwnerTy_, ItemTy_> base_type; \
		base_name() {} \
		explicit base_name(OwnerTy_* owner) : base_type(owner) {} \
		explicit base_name(OwnerTy_* owner, reverse_tag) : base_type(owner, reverse_tag()) {} \
		\
	private: \
		friend class index_iterator_core_access; \
		ItemTy_* first(OwnerTy_* owner, int* pIterIdx, bool bRev = false) const { return owner->first_fn(pIterIdx, bRev); } \
		ItemTy_* next(OwnerTy_* owner, int* pIterIdx, bool bRev = false) const { return owner->next_fn(pIterIdx, bRev); } \
	}; \
	typedef base_name<owner_type, item_type> itr_name; \
	typedef base_name<const owner_type, const item_type> const_itr_name;


#endif // index_iterator_base_h__
