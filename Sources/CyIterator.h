#pragma once

#ifndef CyIterator_h__
#define CyIterator_h__

#include <boost/python/object/iterator_core.hpp>
#include <boost/python/def_visitor.hpp>


struct CovertToTuple
{
	typedef python::tuple return_type;

	template <typename T1, typename T2>
	static return_type convert(const std::pair<T1, T2>& pair)
	{
		return python::make_tuple(static_cast<int>(pair.first), pair.second);
	}
};


template <typename Iterator, typename Converter = CovertToTuple>
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


template <typename Container>
class CyVector : public python::def_visitor<CyVector<Container> >
{
public:
	template <class Class>
	void visit(Class& cl) const
	{
		cl
			.def("__len__", size)
			.def("__getitem__", &get_item)
			.def("__contains__", &contains)
			.def("__iter__", &iter, python::return_value_policy<python::manage_new_object>())
		;
	}

	static size_t size(Container& container)
	{
		return container.size();
	}

	static typename Container::value_type get_item(Container& container, int i)
	{ 
		return container[i];
	}

	static bool contains(Container& container, PyObject* key)
	{
		python::extract<typename Container::value_type const&> x(key);
		return x.check() && algo::contains(container, x());
	}

	static CyIterator<typename Container::iterator>* iter(Container& container)
	{
		return new CyIterator<typename Container::iterator>(container.begin(), container.end());
	}
};


template <typename Vector_t>
void publishPythonVectorInterface()
{
	python::class_<Vector_t>("CyVector", python::no_init)
		.def(CyVector<Vector_t>())
	;
}

#endif