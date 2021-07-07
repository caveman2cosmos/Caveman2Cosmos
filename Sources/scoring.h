#pragma once

#ifndef scoring_h__
#define scoring_h__

// Types and algorithms for helping with scoring.
// Scoring is the process of mapping an item to an integer value
// that represents its value by some set of criteria.
//
// e.g. Scoring a unit based on its ability to defend against another unit.
//
// The preferred way to do scoring is to transform a range of items (of type Ty_) into
// a range of item_score<Ty_>s via a scoring function.
// This range of scored items can then be used as needed.
// e.g.
//   Call algo::max_element() to get the best scoring item.
//   Sort the range from biggest to smallest.
namespace scoring {

	template < class ItemTy_ >
	struct item_score
	{
		item_score(ItemTy_ item = ItemTy_(), int score = 0) : item(item), score(score) {}

		bool operator<(const item_score& other) const { return score < other.score; }

		ItemTy_ item;
		int score;
	};

	namespace detail {
		template < class ItemTy_ >
		struct score_item : std::unary_function< ItemTy_ , item_score<ItemTy_> >
		{
			typedef bst::function<int(const ItemTy_&)> ScoringFn_;
			score_item() {}
			score_item(ScoringFn_ scoringFn) : scoringFn(scoringFn) {}

			item_score<ItemTy_> operator()(ItemTy_ item) const
			{
				return item_score<ItemTy_>(item, scoringFn(item));
			}

			ScoringFn_ scoringFn;
		};
	}

	// Return item with maximum score as determined by scoringFn
	template < class RangeItrTy_ >
	bst::optional<typename bst::range_value<RangeItrTy_>::type> max_score(RangeItrTy_& range, bst::function<int(const typename bst::range_value<RangeItrTy_>::type&)> scoringFn)
	{
		typedef typename bst::range_value<RangeItrTy_>::type value_type;
		bst::optional< item_score<value_type> > best = algo::max_element(range
			| uniqued
			| transformed(detail::score_item<value_type>(scoringFn))
		);
		if (best)
		{
			return best->item;
		}
		return bst::optional<value_type>();
	}

	// Return item with minimum score as determined by scoringFn
	template < class RangeItrTy_ >
	bst::optional<typename bst::range_value<RangeItrTy_>::type> min_score(RangeItrTy_& range, bst::function<int(const typename bst::range_value<RangeItrTy_>::type&)> scoringFn)
	{
		typedef typename bst::range_value<RangeItrTy_>::type value_type;
		bst::optional< item_score<value_type> > best = algo::min_element(range
			| uniqued
			| transformed(detail::score_item<value_type>(scoringFn))
		);
		if (best)
		{
			return best->item;
		}
		return bst::optional<value_type>();
	}

	//template < class RangeItrTy_ >
	//bst::optional< typename RangeItrTy_::value_type > unwrap(const RangeItrTy_& itr)
	//{
	//
	//}
}

#endif // scoring_h__