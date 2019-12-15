#pragma once

#ifndef idinfo_iterator_h__
#define idinfo_iterator_h__

#include "LinkedList.h"

struct IDInfo;

// Copying some of boost iterator facade instead of using it directly as intellisense isn't working with boost.

// This class gets access to the derived iterators internals via friendship.
template < class Derived, class Value >
class idinfo_iterator_core_access
{
public:
	static Value* resolve(const Derived& derived, const IDInfo& info) { return derived.resolve(info); }
};

// A simple forward iterator that moves to the next node preemptively to avoid the current node being invalidated by
// changes done to the referenced unit.
// This might not be necessary, however it is done to safely emulate the code it is replacing which had this 
// characteristic

template < class Derived, class Value >
class idinfo_iterator // : public bst::iterator_facade<unit_iterator, Value, bst::forward_traversal_tag>
{
	// Curiously Recurring Template interface.
	Derived& derived()
	{
		return *static_cast<Derived*>(this);
	}

	Derived const& derived() const
	{
		return *static_cast<Derived const*>(this);
	}

public:
	typedef idinfo_iterator_core_access<Derived, Value> core_access;
	typedef idinfo_iterator<Derived, Value> this_type;
	typedef Value* value_type;
	typedef std::forward_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;
	typedef Value* pointer;
	typedef Value* reference;

public:
	idinfo_iterator() : m_list(NULL), m_node(NULL), m_ptr(NULL) {}
	explicit idinfo_iterator(const CLinkList<IDInfo>* list) : m_list(list), m_node(list->head())
	{
		// We start by incrementing
		increment();
	}
	bool operator==(idinfo_iterator const& other) const { return equal(other); }
	bool operator!=(idinfo_iterator const& other) const { return !equal(other); }
	idinfo_iterator& operator++() { increment(); return *this; }
	idinfo_iterator  operator++(int) { idinfo_iterator prev(*this); increment(); return prev; }
	Value*& operator*() { return dereference(); };
	const Value* operator*() const { return dereference(); };
	Value* operator->() { return dereference(); };

	bool valid() const { return m_ptr != NULL; }

private:
	// friend class idinfo_iterator_core_access;
	// Value* resolve(const IDInfo& info) const;

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

	bool equal(idinfo_iterator const& other) const { return this->m_ptr == other.m_ptr; }
	Value*& dereference() { return m_ptr; }
	Value* dereference() const { return m_ptr; }

	const CLinkList<IDInfo>* m_list;
	CLLNode<IDInfo>* m_node;
	Value* m_ptr;
};

namespace std {

	template < class Derived, class Value >
	struct iterator_traits < idinfo_iterator < Derived, Value > >
	{	// get traits from iterator _Iter
		typedef forward_iterator_tag iterator_category;
		// typedef Value value_type;
		// typedef ptrdiff_t difference_type;
		// typedef difference_type distance_type;	// retained
		// typedef Value* pointer;
		// typedef Value& reference;
	};

}

#endif // idinfo_iterator_h__
