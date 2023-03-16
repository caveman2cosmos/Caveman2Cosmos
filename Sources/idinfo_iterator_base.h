#pragma once

#ifndef idinfo_iterator_h__
#define idinfo_iterator_h__

#include "FProfiler.h"

//#include "LinkedList.h"

struct IDInfo;

template < class Facade, class Value >
class idinfo_iterator_core_access
{
public:
	static Value* resolve(const Facade& facade, const IDInfo& info) { return facade.resolve(info); }
};

// Base class for iteration using indexed iteration functions (CvPlayer::firstCity/nextCity etc.)
template < class Derived, class Value >
class idinfo_iterator_base : public bst::iterator_facade<Derived, Value*, bst::forward_traversal_tag, Value*>
{
public:
	typedef idinfo_iterator_base<Derived, Value> base_type;

	idinfo_iterator_base() : m_list(nullptr), m_node(nullptr), m_ptr(nullptr) {}
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
		PROFILE_EXTRA_FUNC();
		if (m_node == nullptr)
		{
			m_ptr = nullptr;
		}
		else
		{
			do
			{
				m_ptr = core_access::resolve(derived(), m_node->m_data);
				// Immediately increment to the next node so any changes relating to m_ptr can't invalidate the iterator
				m_node = m_list->next(m_node);
			} while (m_ptr == nullptr && m_node != nullptr);
		}
	}

	bool equal(idinfo_iterator_base const& other) const
	{
		return this->m_ptr == other.m_ptr;
	}

	Value* dereference() const { return m_ptr; }

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

#define DECLARE_IDINFO_ITERATOR(item_type, itr_name) \
	class itr_name : public idinfo_iterator_base<itr_name, item_type> \
	{ \
	public: \
		itr_name() {} \
		explicit itr_name(const CLinkList<IDInfo>* list) : base_type(list) {} \
	private: \
		friend class core_access; \
		item_type* resolve(const IDInfo& info) const; \
	};

#endif // idinfo_iterator_h__
