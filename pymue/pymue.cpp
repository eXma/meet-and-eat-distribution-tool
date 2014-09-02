#include <boost/python.hpp>

#include "mue_algorithm.h"
#include "teams.h"
#include "distances.h"

#include "seen_table_wrapper.h"
#include "guest_tuple_iterator_wrapper.h"


#include <vector>


BOOST_PYTHON_MODULE (_pymue)
{
	using namespace boost::python;

	class_<mue::Team>("Team", init<int>())
		.def("id", &mue::Team::id);

	class_<mue::Distance_matrix>("DistanceMatrix", init<unsigned int>())
		.def("set_cost", &mue::Distance_matrix::set_cost)
		.def("lookup", &mue::Distance_matrix::lookup);

	class_<pymue::Seen_table_wrapper>("SeenTable", init<unsigned int>())
		.def("clone", &pymue::Seen_table_wrapper::clone)
		.def("generation", &pymue::Seen_table_wrapper::generation)
		.def("seen", &pymue::Seen_table_wrapper::seen)
		.def("add_meeting", &pymue::Seen_table_wrapper::add_meeting);


	class_<mue::Guest_tuple_generator::GuestPair>("GuestPair", init<mue::Team_id, mue::Team_id>())
		.def_readonly("first",  &mue::Guest_tuple_generator::GuestPair::first)
		.def_readonly("second", &mue::Guest_tuple_generator::GuestPair::second);

	class_<pymue::Guest_tuple_generator_wrapper>("GuestTupleGenerator", init<list, list>())
		.def("__iter__", iterator<pymue::Guest_tuple_generator_wrapper>());

	class_<mue::Calculation>("Calculation", init<unsigned int, mue::Distance_matrix>())
		.def("dummy_distance", &mue::Calculation::dummy_distance);
}
