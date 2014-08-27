#include <boost/python.hpp>

#include "mue_algorithm.h"
#include "teams.h"
#include "distances.h"

#include "seen_table_wrapper.h"



char const * peng() {
	return mue::peng().c_str();
}


BOOST_PYTHON_MODULE (_pymue)
{
	using namespace boost::python;

	def ("peng", peng);

	class_<mue::Team>("Team", init<int>())
		.def("id", &mue::Team::id);

	class_<mue::Distance_matrix>("DistanceMatrix", init<int>())
		.def("set_cost", &mue::Distance_matrix::set_cost)
		.def("lookup", &mue::Distance_matrix::lookup);

	class_<pymue::Seen_table_wrapper>("SeenTable", init<int>())
		.def("clone", &pymue::Seen_table_wrapper::clone)
		.def("generation", &pymue::Seen_table_wrapper::generation)
		.def("seen", &pymue::Seen_table_wrapper::seen)
		.def("add_meeting", &pymue::Seen_table_wrapper::add_meeting);
}
