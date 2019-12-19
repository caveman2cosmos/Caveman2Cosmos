#pragma once

#ifndef algorithm2_h__
#define algorithm2_h__

#include "flatten_range.h"

// We wrap some existing range algorithms here to make them easier to use without auto keyword.
// Instead of returning the complicated iterator type we return an optional of a value for direct use.

// Declare a bst::optional version of an existing algorithm (e.g. bst::find)
#define DECLARE_OPT_RANGE_ALGO(src_algo_name_, opt_algo_name_) \
	template< class Range_ > \
	bst::optional<typename bst::range_value<Range_>::type> opt_algo_name_(Range_ rng) \
	{	 \
		typename bst::range_iterator<Range_>::type itr = src_algo_name_(rng); \
		if (itr == bst::end(rng)) \
		{ \
			return bst::optional<typename bst::range_value<Range_>::type>(); \
		} \
		return *itr; \
	}

// Declare a bst::optional version of an existing algorithm that takes a single argument (e.g. bst::find_if)
#define DECLARE_OPT_RANGE_ALGO_ARG(src_algo_name_, opt_algo_name_) \
	template< class Range_, class Arg_ > \
	bst::optional<typename bst::range_value<Range_>::type> opt_algo_name_(Range_& rng, Arg_& arg) \
	{	 \
		typename bst::range_iterator<Range_>::type itr = src_algo_name_(rng, arg); \
		if (itr == bst::end(rng)) \
		{ \
			return bst::optional<typename bst::range_value<Range_>::type>(); \
		} \
		return *itr; \
	}

// Declares a functor that will store a value to compare against the return value of a specific member function.
// Using this inside a class can provide good readability when it is used
// e.g.
// class CvUnit {
//     ...
//     DECLARE_FILTER_FUNCTOR(CvUnit, has_id, getID, int);
//     ...
// };
// 
// Results in usage like: CvUnit::has_id(anId)
//

#define DECLARE_FILTER_FUNCTOR(obj_type_, name_, mem_fn_, val_type_) \
	struct name_ : std::unary_function<const obj_type_*, bool> \
	{ \
		name_(const name_& other) : val(other.val) {} \
		name_(const val_type_ val = val_type_()) : val(val) {} \
		bool operator()(const obj_type_* obj) const \
		{ \
			return obj->mem_fn_() == val; \
		} \
		val_type_ val; \
	};
#define DECLARE_FILTER_FUNCTOR_P(obj_type_, name_, mem_fn_, val_type_) \
	struct name_ : std::unary_function<const obj_type_*, bool> \
	{ \
		name_(const name_& other) : val(other.val) {} \
		name_(const val_type_ val = val_type_()) : val(val) {} \
		bool operator()(const obj_type_* obj) const \
		{ \
			return obj->mem_fn_(val); \
		} \
		val_type_ val; \
	}; 
#define DECLARE_TRANSFORM_FUNCTOR(obj_type_, name_, result_type_, mem_fn_) \
	struct name_ : std::unary_function<const obj_type_*, result_type_> \
	{ \
		result_type_ operator()(const obj_type_* obj) const \
		{ \
			return obj->mem_fn_(); \
		} \
	};
#define DECLARE_TRANSFORM_FUNCTOR_1(obj_type_, name_, result_type_, mem_fn_, val_type_) \
	struct name_ : std::unary_function<const obj_type_*, result_type_> \
	{ \
		name_(const name_& other) : val(other.val) {} \
		name_(const val_type_ val = val_type_()) : val(val) {} \
		result_type_ operator()(const obj_type_* obj) const \
		{ \
			return obj->mem_fn_(val); \
		} \
		val_type_ val; \
	};
#define DECLARE_TRANSFORM_FUNCTOR_2(obj_type_, name_, result_type_, mem_fn_, val_type_, val_type2_) \
	struct name_ : std::unary_function<const obj_type_*, result_type_> \
	{ \
		name_(const name_& other) : val(other.val), val2(other.val2) {} \
		name_(const val_type_ val = val_type_(), const val_type2_ val2 = val_type2_()) : val(val), val2(val2) {} \
		result_type_ operator()(const obj_type_* obj) const \
		{ \
			return obj->mem_fn_(val, val2); \
		} \
		val_type_ val; \
		val_type2_ val2; \
	};

namespace algo {
	DECLARE_OPT_RANGE_ALGO(bst::range::min_element, min_element);
	DECLARE_OPT_RANGE_ALGO(bst::range::max_element, max_element);
	DECLARE_OPT_RANGE_ALGO_ARG(bst::range::find, find);
	DECLARE_OPT_RANGE_ALGO_ARG(bst::range::find_if, find_if);

	// FUNCTION TEMPLATE contains
	// test if an element exists in a range
	template< class _Range, class Item_ >
	bool contains(const _Range& rng, const Item_& item) {
		typedef typename bst::range_iterator<_Range>::type itr;
		itr _First = bst::begin(rng),
			_Last = bst::end(rng);
		for (; _First != _Last; ++_First) {
			if (*_First == item) {
				return true;
			}
		}
		return false;
	}

	//template< class Range_ >
	//bst::optional<typename Range_::value_type> max_element(Range_ rng)
	//{	
	//	// find largest element, using operator<
	//	typename Range_::iterator itr = bst::max_element(rng);
	//	if (itr == bst::range::end(rng))
	//	{
	//		return bst::optional<typename Range_::value_type>();
	//	}
	//	return *itr;
	//}

	template< class Ty_ >
	bool exists(const bst::optional<Ty_>& optval)
	{
		return optval;
	}

	template< class Ty_ >
	const Ty_& get(const bst::optional<Ty_>& optval)
	{
		return optval.get();
	}
	//namespace detail {
	//	template < class ItemTy_, class ScoringFn_ >
	//	item_score<ItemTy_> score(ItemTy_& item, const ScoringFn_& scoringFn)
	//	{
	//		return item_score(item, scoringFn(item));
	//	}
	//}

	//template < class Range_, class ScoringFn_ >
	// score(const Range_& range, const ScoringFn_& scoringFn)
	//{
	//	return range | bst::adaptors::transformed(bst::bind(score, _1, scoringFn));
	//}
	//	
	
	// FUNCTION TEMPLATE all_of
	// test if all elements are true
	template< class _Range >
	bool all_of(const _Range& rng) {
		typedef typename bst::range_iterator<_Range>::type itr;
		itr _First = bst::begin(rng),
			_Last = bst::end(rng);
		for (; _First != _Last; ++_First) {
			if (!*_First) {
				return false;
			}
		}
		return true;
	}
	// FUNCTION TEMPLATE all_of
	// test if all elements satisfy _Pred
	template< class _Range, class _Pr >
	bool all_of(const _Range& rng, _Pr& _Pred) {
		typedef typename bst::range_iterator<_Range>::type itr;
		itr _First = bst::begin(rng),
			_Last = bst::end(rng);
		for (; _First != _Last; ++_First) {
			if (!_Pred(*_First)) {
				return false;
			}
		}
		return true;
	}

	// FUNCTION TEMPLATE any_of
	// test if any element satisfies _Pred
	template< class _Range>
	bool any_of(const _Range& rng) {
		typedef typename bst::range_iterator<_Range>::type itr;
		itr _First = bst::begin(rng),
			_Last = bst::end(rng);
		for (; _First != _Last; ++_First) {
			if (*_First) {
				return true;
			}
		}
		return false;
	}

	// FUNCTION TEMPLATE any_of
	// test if any element satisfies _Pred
	template< class _Range, class _Pr>
	bool any_of(const _Range& rng, _Pr& _Pred) {
		typedef typename bst::range_iterator<_Range>::type itr;
		itr _First = bst::begin(rng),
			_Last = bst::end(rng);
		for (; _First != _Last; ++_First) {
			if (_Pred(*_First)) {
				return true;
			}
		}
		return false;
	}

	// FUNCTION TEMPLATE none_of
	// test if no elements satisfy _Pred
	template< class _Range>
	bool none_of(const _Range& rng) {
		return !any_of(rng);
	}

	// FUNCTION TEMPLATE none_of
	// test if no elements satisfy _Pred
	template< class _Range, class _Pr>
	bool none_of(const _Range& rng, _Pr& _Pred) {
		return !any_of(rng, _Pred);
	}

	// FUNCTION TEMPLATE count_all
	// return number of items in a range, specialized for ranges with random_access_traversal_tag
	template< class _Range >
	typename bst::enable_if<
		typename bst::is_convertible<
			typename bst::iterator_category_to_traversal<typename bst::range_iterator<_Range>::type::iterator_category>::type,
			bst::random_access_traversal_tag
		>,
		size_t
	>::type
	count_all(const _Range& rng) {
		return bst::size(rng);
	}

	template < class _Range >
	typename bst::disable_if<
		typename bst::is_convertible<
			typename bst::iterator_category_to_traversal<typename bst::range_iterator<_Range>::type::iterator_category>::type,
			bst::random_access_traversal_tag
		>,
		size_t
	>::type 
	count_all(const _Range& rng) {
		typedef typename bst::range_iterator<_Range>::type itr;
		itr _First = bst::begin(rng), _Last = bst::end(rng);
		size_t size = 0;
		for (; _First != _Last; ++_First, ++size) {}
		return size;
	}

	//// FUNCTION TEMPLATE copy_if
	//// copy each satisfying _Pred
	//template <class _InIt, class _OutIt, class _Pr>
	//_OutIt copy_if(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred) { 
	//	for (; _First != _Last; ++_First) {
	//		if (_Pred(*_First)) {
	//			*_Dest = *_First;
	//			++_Dest;
	//		}
	//	}
	//	return _Dest;
	//}
}

//namespace std {
//	// FUNCTION TEMPLATE all_of
//	template <class _InIt, class _Pr>
//	bool all_of(_InIt _First, _InIt _Last, _Pr _Pred) { // test if all elements satisfy _Pred
//		for (; _First != _Last; ++_First) {
//			if (!_Pred(*_First)) {
//				return false;
//			}
//		}
//
//		return true;
//	}
//
//	// FUNCTION TEMPLATE any_of
//	template <class _InIt, class _Pr>
//	bool any_of(_InIt _First, const _InIt _Last, _Pr _Pred) { // test if any element satisfies _Pred
//		
//		for (; _First != _Last; ++_First) {
//			if (_Pred(*_First)) {
//				return true;
//			}
//		}
//		return false;
//	}
//
//	// FUNCTION TEMPLATE none_of
//	template <class _InIt, class _Pr>
//	bool none_of(_InIt _First, const _InIt _Last, _Pr _Pred) { // test if no elements satisfy _Pred
//		for (; _First != _Last; ++_First) {
//			if (_Pred(*_First)) {
//				return false;
//			}
//		}
//
//		return true;
//	}
//
//	// FUNCTION TEMPLATE copy_if
//	template <class _InIt, class _OutIt, class _Pr>
//	_OutIt copy_if(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred) { // copy each satisfying _Pred
//		for (; _First != _Last; ++_First) {
//			if (_Pred(*_First)) {
//				*_Dest = *_First;
//				++_Dest;
//			}
//		}
//		return _Dest;
//	}
//};

#endif // algorithm2_h__
