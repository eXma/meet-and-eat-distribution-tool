#include <boost/python.hpp>

#include "mue_algorithm.h"


char const * peng() {
	return mue::peng().c_str();
}


BOOST_PYTHON_MODULE (_pymue)
{
	using namespace boost::python;

	def ("peng", peng);
}
