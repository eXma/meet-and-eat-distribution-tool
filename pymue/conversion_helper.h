#ifndef PYMUE_CONVERSION_HELPER
#define PYMUE_CONVERSION_HELPER

#include <vector>
#include <unordered_set>

#include <boost/python.hpp>
#include <boost/python/list.hpp>


namespace pymue {
	template <typename T>
	std::unordered_set<T> translate_set(boost::python::list const &l)
	{
		std::unordered_set<T> new_set;

		for (int i= 0; i < boost::python::len(l); ++i) {
			new_set.insert(boost::python::extract<T>(l[i]));
		}

		return new_set;
	}

	template <typename T>
	std::vector<T> translate_vector(boost::python::list const &l)
	{
		std::vector<T> new_vec;

		for (int i= 0; i < boost::python::len(l); ++i) {
			new_vec.push_back(boost::python::extract<T>(l[i]));
		}

		return new_vec;
	}
}

#endif

