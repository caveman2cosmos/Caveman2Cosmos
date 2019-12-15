#pragma once

#ifndef idinfo_iterator_h__
#define idinfo_iterator_h__

#include "LinkedList.h"

struct IDInfo;

template < class Facade, class Value >
class idinfo_iterator_core_access
{
public:
	static typename Facade::value_type* resolve(const Facade facade, const IDInfo& info) { return facade.resolve(info); }
};

// Base class for iteration using indexed iteration functions (CvPlayer::firstCity/nextCity etc.)
template < class Derived, class Value >
class idinfo_iterator_base : public bst::iterator_facade<Derived, Value, bst::forward_traversal_tag, Value&>
{
public:
	typedef Value value_type;
	typedef idinfo_iterator_base<Derived, Value> base_type;

	idinfo_iterator_base() : m_list(NULL), m_node(NULL), m_ptr(NULL) {}
	explicit idinfo_iterator_base(const CLinkList<IDInfo>* list) : m_list(list), m_node(list->head())
	{
		// We start by incrementing
		increment();
	}

private:
	typedef idinfo_iterator_core_access<Derived, Value> core_access;
	friend class bst::iterator_core_access;

	void increment()
	{
		if (m_node == NULL)
		{
			m_ptr = NULL;
		}
		else
		{
			do
			{
				m_ptr = core_access::resolve(derived(), m_node->m_data);
				// Immediately increment to the next node so any changes relating to m_ptr can't invalidate the iterator
				m_node = m_list->next(m_node);
			} while (m_ptr == NULL && m_node != NULL);
		}
	}
	
	bool equal(idinfo_iterator_base const& other) const
	{
		return this->m_ptr == other.m_ptr;
	}
	
	Value& dereference() const { return *m_ptr; }

	Derived& derived()
	{
		return *static_cast<Derived*>(this);
	}

	Derived const& derived() const
	{
		return *static_cast<Derived const*>(this);
	}

	const CLinkList<IDInfo>* m_list;
	CLLNode<IDInfo>* m_node;
	Value* m_ptr;
};

#define DECLARE_IDINFO_ITERATORS(item_type, base_name, itr_name, const_itr_name, first_fn, next_fn) \
	template < class ItemTy_ > \
	class base_name : public idinfo_iterator_base<base_name<ItemTy_>, ItemTy_> \
	{ \
	public: \
		typedef index_iterator_base<base_name<ItemTy_>, ItemTy_> base_type; \
		base_name() {} \
		explicit base_name(OwnerTy_* owner) : base_type(owner) {} \
		explicit base_name(OwnerTy_* owner, reverse_tag) : base_type(owner, reverse_tag()) {} \
		\
	private: \
		friend class index_iterator_core_access; \
		ItemTy_* first(OwnerTy_* owner, int* pIterIdx, bool bRev = false) const { return owner->first_fn(pIterIdx, bRev); } \
		ItemTy_* next(OwnerTy_* owner, int* pIterIdx, bool bRev = false) const { return owner->next_fn(pIterIdx, bRev); } \
	}; \
	typedef base_name<item_type> itr_name; \
	typedef base_name<const item_type> const_itr_name;


#endif // idinfo_iterator_h__
