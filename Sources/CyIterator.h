#pragma once

#ifndef CyIterator_h__
#define CyIterator_h__

#include <boost/python/object/iterator_core.hpp>
#include <boost/python/def_visitor.hpp>


const struct CovertToInteger
{
	typedef int return_type;

	template <typename T>
	static return_type convert(const T& item)
	{
		return static_cast<return_type>(item);
	}
};

const struct CovertToTuple
{
	typedef python::tuple return_type;

	template <typename T1, typename T2>
	static return_type convert(const std::pair<T1, T2>& pair)
	{
		return python::make_tuple(static_cast<int>(pair.first), pair.second);
	}
};


template <typename Iterator, typename Converter>
class CyIterator
{
public:
	CyIterator(const Iterator& begin, const Iterator& end)
		: m_cur(begin)
		, m_end(end)
	{}

	CyIterator* __iter__()
	{
		return this;
	}

	typename Converter::return_type next()
	{
		if (m_cur == m_end)
			python::objects::stop_iteration_error();
		return Converter::convert(*m_cur++);
	}

private:
	Iterator m_cur;
	Iterator m_end;
};


template <typename Iterator>
void publishPythonIteratorInterface()
{
	python::class_<Iterator>("CyIterator", python::no_init)
		//.def("__iter__", &Iterator::__iter__, python::return_value_policy<python::reference_existing_object>())
		.def("next", &Iterator::next)
	;
}


template <typename Container, typename Converter>
class CyVector : public python::def_visitor<CyVector<Container, Converter> >
{
public:
	template <class Class>
	void visit(Class& cl) const
	{
		cl
			.def("__len__", size)
			.def("__getitem__", &getItem)
			.def("__contains__", &contains)
			.def("__iter__", &iter, python::return_value_policy<python::manage_new_object>())
		;
	}

	static int size(Container& container)
	{
		return static_cast<int>(container.size());
	}

	static typename Container::value_type getItem(Container& container, int i)
	{
		FASSERT_BOUNDS(0, size(container), i);
		return container[i];
	}

	static bool contains(Container& container, typename Container::value_type item)
	{
		return algo::any_of_equal(container, item);
	}

	typedef CyIterator<typename Container::iterator, Converter>  python_iterator;

	static python_iterator* iter(Container& container)
	{
		return new python_iterator(container.begin(), container.end());
	}
};


template <typename Container, typename Converter>
void publishPythonVectorInterface()
{
	python::class_<Container>("CyVector", python::no_init)
		.def(CyVector<Container, Converter>())
	;

	publishPythonIteratorInterface<CyIterator<typename Container::iterator, Converter> >();
}

#endif