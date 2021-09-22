#pragma once

#ifndef CyIterator_h__
#define CyIterator_h__

#include <boost/python/object/iterator_core.hpp>

template <typename Container>
class CyIterator
{
public:
	CyIterator(typename Container::iterator& cur, typename Container::iterator& end)
		: m_cur(cur)
		, m_end(end)
	{}

	//CyIterator* __iter__()
	//{
	//	return this;
	//}

	typename Container::value_type& next()
	{
		if (m_cur == m_end)
			python::objects::stop_iteration_error();
		return *m_cur++;
	}

private:
	typename Container::iterator m_cur;
	typename Container::iterator m_end;
};

template <typename Container>
void publishPythonIteratorInterface()
{
	python::class_<CyIterator<Container> >("CyIterator", python::no_init)
		//.def("__iter__", &CyIterator<Container>::__iter__)
		.def("next", &CyIterator<Container>::next, python::return_value_policy<python::reference_existing_object>())
	;
}

#endif