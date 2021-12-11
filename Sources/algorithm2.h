#pragma once

#ifndef algorithm2_h__
#define algorithm2_h__

#include "flatten_range.h"
#include <boost155/algorithm/cxx11/all_of.hpp>
#include <boost155/algorithm/cxx11/any_of.hpp>
#include <boost155/algorithm/cxx11/copy_if.hpp>
#include <boost155/algorithm/cxx11/find_if_not.hpp>
#include <boost155/algorithm/cxx11/none_of.hpp>

/// We wrap some existing range algorithms here to make them easier to use without auto keyword.
/// Instead of returning the complicated iterator type we return an optional of a value for direct use.

/// Declare a bst::optional version of an existing algorithm (e.g. bst::find)
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

/// Declare a bst::optional version of an existing algorithm that takes a single argument (e.g. bst::find_if)
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

namespace detail {

	template < class ObjType_, class ReturnType_, class Derived_ >
	struct algo_functor : std::unary_function<ObjType_, ReturnType_>
	{
		typedef Derived_ derived;
	};

	template <class T>
	struct is_algo_functor : bst::integral_constant<bool, false>
	{
		static const bool value = false;
	};

	template <class O, class R, class D>
	struct is_algo_functor< algo_functor<O, R, D> > : bst::integral_constant<bool, true>
	{
		static const bool value = true;
	};

	// operator!
	template < class ObjType_, class Derived_ >
	struct logical_not : algo_functor< ObjType_, bool, logical_not<ObjType_, Derived_> >
	{
		logical_not(Derived_ fn = Derived_()) : fn(fn) {}
		template<class V> bool operator()(V const& v) const { return !fn(v); }
		Derived_ fn;
	};

	template<class O, class R, class D>
	logical_not<O, D>
	operator! (const algo_functor<O, R, D>& f)
	{
		return logical_not<O, D>(static_cast<const D&>(f));
	}

	/// relational operators
#define DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR( op, name ) \
	template < class ObjType_, class Derived1_, class Derived2_ > \
	struct name : algo_functor< ObjType_, bool, name<ObjType_, Derived1_, Derived2_> > \
	{ \
		name(Derived1_ fn1 = Derived1_(), Derived2_ fn2 = Derived2_()) : fn1(fn1), fn2(fn2) {} \
		template<class V> bool operator()(const V& v) const { return fn1(v) op fn2(v); } \
		Derived1_ fn1; \
		Derived2_ fn2; \
	}; \
	template<class O, class R1, class R2, class D1, class D2> \
	name<O, D1, D2> \
	operator op (const algo_functor<O, R1, D1>& f1, const algo_functor<O, R2, D2>& f2) \
	{ \
		return name<O, D1, D2>(static_cast<const D1&>(f1), static_cast<const D2&>(f2)); \
	}

	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(== , equal);
	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(!= , not_equal);

	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(< , less);
	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(<= , less_equal);

	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(> , greater);
	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(>= , greater_equal);

	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(&&, logical_and);
	DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR(|| , logical_or);

#undef DEFINE_ALGO_FUNCTOR_FUNCTOR_OPERATOR

#define DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR( op, name ) \
	template < class ObjType_, class Derived_, class Value_ > \
	struct name : algo_functor< ObjType_, bool, name<ObjType_, Derived_, Value_> > \
	{ \
		name(Derived_ fn = Derived_(), Value_ val = Value_()) : fn(fn), val(val) {} \
		template<class V> bool operator()(const V& v) const { return fn(v) op val; } \
		Derived_ fn; \
		Value_ val; \
	}; \
	template<class O, class R, class D, class V> \
	typename bst::enable_if< \
		bst::is_convertible< V, R >, \
		name<O, D, V> \
	>::type \
	operator op (const algo_functor<O, R, D>& f, const V& val) \
	{ \
		return name<O, D, V>(static_cast<const D&>(f), val); \
	}

	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(== , val_equal);
	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(!= , val_not_equal);

	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(< , val_less);
	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(<= , val_less_equal);

	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(> , val_greater);
	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(>= , val_greater_equal);

	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(&&, val_logical_and);
	DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR(|| , val_logical_or);

#undef DEFINE_ALGO_FUNCTOR_VALUE_OPERATOR

}

///
/// Declares a functor that will can be used in range adaptors, and algorithms.
/// e.g.
/// class CvUnit {
///     ...
///     struct algo
///     {
///         DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, getID);
///     };
///     ...
/// };
///
/// Results in usage like:
///   CvUnit::fn::getID()
///
/// The functors generated can be combined using comparison and boolean operators
/// defined above like so:
///   CvUnit::fn::getID() == 1 && CvUnit::fn::getTeam() != NO_TEAM
/// or
///   CvUnit::fn::getDamage() < CvUnit::fn::getHealth()
///
namespace map_fun_details {

	template < class Ty_, class Enable_ = void >
	struct default_value
	{
		static const Ty_ value = Ty_();
	};

	template < class Ty_ >
	struct default_value<Ty_, typename bst::enable_if< bst::is_integral<Ty_> >::type>
	{
		static const Ty_ value = Ty_();
	};

	template < class Ty_ >
	struct default_value<Ty_, typename bst::enable_if< bst::is_pointer<Ty_> >::type>
	{
		static const Ty_ value;
	};

	template < class Ty_ >
	const Ty_ default_value<Ty_, typename bst::enable_if< bst::is_pointer<Ty_> >::type>::value = nullptr;
}

#define DECLARE_MAP_FUNCTOR(obj_type_, result_type_, mem_fn_) \
	struct mem_fn_ : detail::algo_functor<obj_type_*, result_type_, mem_fn_> { \
		result_type_ operator()(obj_type_* obj) { \
			return obj->mem_fn_(); \
		} \
	};
#define DECLARE_MAP_FUNCTOR_1(obj_type_, result_type_, mem_fn_, val_type_) \
	struct mem_fn_ : detail::algo_functor<obj_type_*, result_type_, mem_fn_> { \
		mem_fn_(mem_fn_& other) : val(other.val) {} \
		mem_fn_(val_type_ val = map_fun_details::default_value<val_type_>::value) : val(val) {} \
		result_type_ operator()(obj_type_* obj) { \
			return obj->mem_fn_(val); \
		} \
		val_type_ val; \
	};
#define DECLARE_MAP_FUNCTOR_2(obj_type_, result_type_, mem_fn_, val_type_, val_type2_) \
	struct mem_fn_ : detail::algo_functor<obj_type_*, result_type_, mem_fn_> { \
		mem_fn_(mem_fn_& other) : val(other.val), val2(other.val2) {} \
		mem_fn_(val_type_ val = map_fun_details::default_value<val_type_>::value, val_type2_ val2 = map_fun_details::default_value<val_type2_>::value) : val(val), val2(val2) {} \
		result_type_ operator()(obj_type_* obj) { \
			return obj->mem_fn_(val, val2); \
		} \
		val_type_ val; \
		val_type2_ val2; \
	};
#define DECLARE_MAP_FUNCTOR_3(obj_type_, result_type_, mem_fn_, val_type_, val_type2_, val_type3_) \
	struct mem_fn_ : detail::algo_functor<const obj_type_*, result_type_, mem_fn_> { \
		mem_fn_(mem_fn_& other) : val(other.val), val2(other.val2), val3(other.val3) {} \
		mem_fn_(val_type_ val = map_fun_details::default_value<val_type_>::value, val_type2_ val2 = map_fun_details::default_value<val_type2_>::value, val_type3_ val3 = map_fun_details::default_value<val_type3_>::value) : val(val), val2(val2), val3(val3) {} \
		result_type_ operator()(obj_type_* obj) { \
			return obj->mem_fn_(val, val2, val3); \
		} \
		val_type_ val; \
		val_type2_ val2; \
		val_type3_ val3; \
	};

#define DECLARE_MAP_FUNCTOR_CONST(obj_type_, result_type_, mem_fn_) \
	struct mem_fn_ : detail::algo_functor<const obj_type_*, result_type_, mem_fn_> { \
		result_type_ operator()(const obj_type_* obj) const { \
			return obj->mem_fn_(); \
		} \
	};
#define DECLARE_MAP_FUNCTOR_CONST_1(obj_type_, result_type_, mem_fn_, val_type_) \
	struct mem_fn_ : detail::algo_functor<const obj_type_*, result_type_, mem_fn_> { \
		mem_fn_(const mem_fn_& other) : val(other.val) {} \
		mem_fn_(val_type_ val = map_fun_details::default_value<val_type_>::value) : val(val) {} \
		result_type_ operator()(const obj_type_* obj) const { \
			return obj->mem_fn_(val); \
		} \
		val_type_ val; \
	};
#define DECLARE_MAP_FUNCTOR_CONST_2(obj_type_, result_type_, mem_fn_, val_type_, val_type2_) \
	struct mem_fn_ : detail::algo_functor<const obj_type_*, result_type_, mem_fn_> { \
		mem_fn_(const mem_fn_& other) : val(other.val), val2(other.val2) {} \
		mem_fn_(val_type_ val = map_fun_details::default_value<val_type_>::value, val_type2_ val2 = map_fun_details::default_value<val_type2_>::value) : val(val), val2(val2) {} \
		result_type_ operator()(const obj_type_* obj) const { \
			return obj->mem_fn_(val, val2); \
		} \
		val_type_ val; \
		val_type2_ val2; \
	};
#define DECLARE_MAP_FUNCTOR_CONST_3(obj_type_, result_type_, mem_fn_, val_type_, val_type2_, val_type3_) \
	struct mem_fn_ : detail::algo_functor<const obj_type_*, result_type_, mem_fn_> { \
		mem_fn_(const mem_fn_& other) : val(other.val), val2(other.val2), val3(other.val3) {} \
		mem_fn_(val_type_ val = map_fun_details::default_value<val_type_>::value, val_type2_ val2 = map_fun_details::default_value<val_type2_>::value, val_type3_ val3 = map_fun_details::default_value<val_type3_>::value) : val(val), val2(val2), val3(val3) {} \
		result_type_ operator()(const obj_type_* obj) const { \
			return obj->mem_fn_(val, val2, val3); \
		} \
		val_type_ val; \
		val_type2_ val2; \
		val_type3_ val3; \
	};

namespace algo {
	/// Bring in boost range algorithms we want

	/// Mutating
	using bst::copy;
	using bst::copy_backward;
	using bst::algorithm::copy_if;
	using bst::fill;
	using bst::fill_n;
	using bst::generate;
	using bst::inplace_merge;
	using bst::merge;
	using bst::nth_element;
	using bst::partial_sort;
	using bst::partition;
	using bst::random_shuffle;
	using bst::remove;
	using bst::remove_copy;
	using bst::remove_copy_if;
	using bst::remove_if;
	using bst::replace;
	using bst::replace_copy;
	using bst::replace_copy_if;
	using bst::replace_if;
	using bst::reverse;
	using bst::reverse_copy;
	using bst::rotate;
	using bst::rotate_copy;
	using bst::sort;
	using bst::stable_partition;
	using bst::stable_sort;
	//using bst::swap_ranges;
	using bst::transform;
	using bst::unique;
	using bst::unique_copy;

	/// Non mutating
	//using bst::adjacent_find;
	//using bst::binary_search;
	using bst::algorithm::all_of;
	using bst::algorithm::all_of_equal;
	using bst::algorithm::any_of;
	using bst::algorithm::any_of_equal;
	using bst::count;
	using bst::count_if;
	using bst::equal;
	using bst::equal_range;
	using bst::for_each;
	//using bst::find;
	DECLARE_OPT_RANGE_ALGO_ARG(bst::range::find, find);
	//using bst::find_end;
	//using bst::find_first_of;
	//using bst::find_if;
	DECLARE_OPT_RANGE_ALGO_ARG(bst::range::find_if, find_if);
	using bst::algorithm::find_if_not;
	using bst::lexicographical_compare;
	using bst::lower_bound;
	//using bst::max_element;
	DECLARE_OPT_RANGE_ALGO(bst::range::max_element, max_element);
	DECLARE_OPT_RANGE_ALGO_ARG(bst::range::max_element, max_element);
	//using bst::min_element;
	DECLARE_OPT_RANGE_ALGO(bst::range::min_element, min_element);
	DECLARE_OPT_RANGE_ALGO_ARG(bst::range::min_element, min_element);
	using bst::mismatch;
	using bst::algorithm::none_of;
	using bst::algorithm::none_of_equal;
	using bst::search;
	using bst::search_n;
	using bst::upper_bound;

	/// Set algorithms
	using bst::includes;
	using bst::set_union;
	using bst::set_intersection;
	using bst::set_difference;
	using bst::set_symmetric_difference;

	/// Numeric algorithms;
	using bst::accumulate;
	using bst::adjacent_difference;
	using bst::inner_product;
	using bst::partial_sum;

	/// Other
	using bst::push_back;
}

#endif // algorithm2_h__
