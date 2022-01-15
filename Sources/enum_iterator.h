#pragma once

namespace range_detail
{
	template<typename EnumType, typename UnderlyingType>
	class enum_iterator
		: public bst::iterator_facade<
			enum_iterator<EnumType, UnderlyingType>,
			EnumType,
			bst::random_access_traversal_tag,
			EnumType,
			std::ptrdiff_t
		>
	{
		typedef bst::iterator_facade<
			enum_iterator<EnumType, UnderlyingType>,
			EnumType,
			bst::random_access_traversal_tag,
			EnumType,
			std::ptrdiff_t
		> base_t;
	public:
		typedef typename base_t::value_type value_type;
		typedef typename base_t::difference_type difference_type;
		typedef typename base_t::reference reference;

		enum_iterator() : m_value() {}
		explicit enum_iterator(value_type x) : m_value(x) {}

	private:
		void increment()
		{
			++m_value;
		}

		void decrement()
		{
			--m_value;
		}

		void advance(difference_type offset)
		{
			m_value += offset;
		}

		difference_type distance_to(const enum_iterator& other) const
		{
			return other.m_value - m_value;
		}

		bool equal(const enum_iterator& other) const
		{
			return m_value == other.m_value;
		}

		reference dereference() const
		{
			return static_cast<value_type>(m_value);
		}

		friend bst::iterator_core_access;
		UnderlyingType m_value;
	};
} // namespace range_detail

template<typename EnumType, typename UnderlyingType>
class enum_range
	: public bst::iterator_range< range_detail::enum_iterator<EnumType, UnderlyingType> >
{
	typedef range_detail::enum_iterator<EnumType, UnderlyingType> iterator_t;
	typedef bst::iterator_range<iterator_t> base_t;
public:
	enum_range(EnumType first, EnumType last)
		: base_t(iterator_t(first), iterator_t(last))
	{
	}
};

template<typename EnumType, typename UnderlyingType, int First, int Last>
class fixed_enum_range
	: public bst::iterator_range< range_detail::enum_iterator<EnumType, UnderlyingType> >
{
	typedef range_detail::enum_iterator<EnumType, UnderlyingType> iterator_t;
	typedef bst::iterator_range<iterator_t> base_t;
public:
	fixed_enum_range()
		: base_t(iterator_t(static_cast<EnumType>(First)), iterator_t(static_cast<EnumType>(Last)))
	{
	}
};