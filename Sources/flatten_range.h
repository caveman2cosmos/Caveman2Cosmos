#pragma once

#ifndef flatten_range_h__
#define flatten_range_h__

#include "FProfiler.h"

#include <boost155/range/adaptors.hpp>
#include <boost155/range/iterator_range.hpp>

template <typename OuterIterator> class flattening_iterator
{
public:
	typedef OuterIterator outer_iterator;
	typedef typename OuterIterator::value_type::iterator inner_iterator;

	typedef std::forward_iterator_tag iterator_category;
	typedef typename inner_iterator::value_type value_type;
	typedef typename inner_iterator::difference_type difference_type;
	typedef typename inner_iterator::pointer pointer;
	typedef typename inner_iterator::reference reference;

	flattening_iterator()
	{
	}
	flattening_iterator(outer_iterator it)
		: outer_it_(it)
		, outer_end_(it)
	{
	}
	flattening_iterator(outer_iterator it, outer_iterator end)
		: outer_it_(it)
		, outer_end_(end)
	{
		if (outer_it_ == outer_end_)
		{
			return;
		}

		inner_it_ = outer_it_->begin();
		advance_past_empty_inner_containers();
	}

	reference operator*() const
	{
		return *inner_it_;
	}
	pointer operator->() const
	{
		return &*inner_it_;
	}

	flattening_iterator& operator++()
	{
		++inner_it_;
		if (inner_it_ == outer_it_->end())
			advance_past_empty_inner_containers();
		return *this;
	}

	flattening_iterator operator++(int)
	{
		flattening_iterator it(*this);
		++* this;
		return it;
	}

	friend bool operator==(const flattening_iterator& a, const flattening_iterator& b)
	{
		if (a.outer_it_ != b.outer_it_)
			return false;

		if (a.outer_it_ != a.outer_end_ && b.outer_it_ != b.outer_end_ && a.inner_it_ != b.inner_it_)
			return false;

		return true;
	}

	friend bool operator!=(const flattening_iterator& a, const flattening_iterator& b)
	{
		return !(a == b);
	}

private:
	void advance_past_empty_inner_containers()
	{
		PROFILE_EXTRA_FUNC();
		while (outer_it_ != outer_end_ && inner_it_ == outer_it_->end())
		{
			++outer_it_;
			if (outer_it_ != outer_end_)
				inner_it_ = outer_it_->begin();
		}
	}

	outer_iterator outer_it_;
	outer_iterator outer_end_;
	inner_iterator inner_it_;
};

template <typename Iterator> flattening_iterator<Iterator> flatten(Iterator it)
{
	return flattening_iterator<Iterator>(it, it);
}

template <typename Iterator> flattening_iterator<Iterator> flatten(Iterator first, Iterator last)
{
	return flattening_iterator<Iterator>(first, last);
}

template <typename R>
struct flat_range : boost155::iterator_range<flattening_iterator<typename boost155::range_iterator<R>::type> >
{
public:
	typedef flattening_iterator<typename boost155::range_iterator<R>::type> iterator;

private:
	typedef boost155::iterator_range<iterator> base;

public:
	flat_range(R& r)
		: base(iterator(boost155::begin(r), boost155::end(r)), iterator(boost155::end(r)))
	{
	}
};

namespace detail
{
	struct flat_forwarder
	{
	};
};

template <class R> inline flat_range<R> operator|(R& r, detail::flat_forwarder)
{
	BOOST155_RANGE_CONCEPT_ASSERT((boost155::SinglePassRangeConcept<R>));

	return flat_range<R>(r);
}

template <class R> inline flat_range<const R> operator|(const R& r, detail::flat_forwarder)
{
	BOOST155_RANGE_CONCEPT_ASSERT((boost155::SinglePassRangeConcept<const R>));

	return flat_range<const R>(r);
}

namespace
{
	const detail::flat_forwarder flattened = detail::flat_forwarder();
}

#endif // flatten_range_h__