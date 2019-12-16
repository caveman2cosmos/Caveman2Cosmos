#pragma once

#ifndef algorithm2_h__
#define algorithm2_h__

namespace {

	template < class ItemTy_ >
	struct item_score
	{
		item_score(ItemTy_ item = ItemTy_(), int score = 0) : item(item), score(score) {}

		bool operator<(const item_score& other) const { return score < other.score; }

		ItemTy_ item;
		int score;
	};

	template < class RangeItrTy_ >
	bst::optional< RangeItrTy_::value_type > unwrap(const RangeItrTy_& itr)
	{

	}

	template<class _FwdIt> inline
	bst::optional<_FwdIt::value_type> max_element(_FwdIt _First, _FwdIt _Last)
	{	// find largest element, using operator<
		_FwdIt _Found = _First;
		if (_First != _Last)
			for (; ++_First != _Last; )
				if (*_Found < *_First)
					_Found = _First;
		return (_Found);
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
