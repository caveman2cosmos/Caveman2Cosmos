#pragma once

#ifndef algorithm2_h__
#define algorithm2_h__

namespace std {
	// FUNCTION TEMPLATE all_of
	template <class _InIt, class _Pr>
	bool all_of(_InIt _First, _InIt _Last, _Pr _Pred) { // test if all elements satisfy _Pred
		for (; _First != _Last; ++_First) {
			if (!_Pred(*_First)) {
				return false;
			}
		}

		return true;
	}

	// FUNCTION TEMPLATE any_of
	template <class _InIt, class _Pr>
	bool any_of(_InIt _First, const _InIt _Last, _Pr _Pred) { // test if any element satisfies _Pred
		
		for (; _First != _Last; ++_First) {
			if (_Pred(*_First)) {
				return true;
			}
		}
		return false;
	}

	// FUNCTION TEMPLATE none_of
	template <class _InIt, class _Pr>
	bool none_of(_InIt _First, const _InIt _Last, _Pr _Pred) { // test if no elements satisfy _Pred
		for (; _First != _Last; ++_First) {
			if (_Pred(*_First)) {
				return false;
			}
		}

		return true;
	}

	// FUNCTION TEMPLATE copy_if
	template <class _InIt, class _OutIt, class _Pr>
	_OutIt copy_if(_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred) { // copy each satisfying _Pred
		for (; _First != _Last; ++_First) {
			if (_Pred(*_First)) {
				*_Dest = *_First;
				++_Dest;
			}
		}
		return _Dest;
	}
};

#endif // algorithm2_h__
