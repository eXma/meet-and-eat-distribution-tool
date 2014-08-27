#include <boost/python.hpp>

#include "mue_algorithm.h"
#include "teams.h"
#include "distances.h"
#include "seen_table.h"


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

	class_<mue::Seen_table>("SeenTable", init<int>())
		.def("clone", &mue::Seen_table::clone)
		.def("generation", &mue::Seen_table::generation)
		.def("seen", &mue::Seen_table::seen)
		.def("add_meeting", &mue::Seen_table::add_meeting);
}
