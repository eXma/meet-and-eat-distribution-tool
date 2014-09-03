#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

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

	class_<std::vector<mue::Team_id> >("TeamVector")
		.def(vector_indexing_suite<std::vector<mue::Team_id> >());

	class_<mue::Guest_tuple_generator::GuestPair>("GuestPair", init<mue::Team_id, mue::Team_id>())
		.def_readonly("first",  &mue::Guest_tuple_generator::GuestPair::first)
		.def_readonly("second", &mue::Guest_tuple_generator::GuestPair::second);

	class_<pymue::Guest_tuple_generator_wrapper>("GuestTupleGenerator", init<list, list>())
		.def("__iter__", iterator<pymue::Guest_tuple_generator_wrapper>());

	class_<mue::Calculation::Round_data>("RoundData", init<>())
		.def_readonly("round", &mue::Calculation::Round_data::round)
		.def_readonly("hosts", &mue::Calculation::Round_data::hosts)
		.def("first_round", &mue::Calculation::Round_data::first_round)
		.def("prev_host", &mue::Calculation::Round_data::prev_host);

	class_<mue::Calculation::Iteration_data>("IterationData", init<unsigned int>())
		.def(init<mue::Calculation::Iteration_data const>())
		.def("seen", &mue::Calculation::Iteration_data::seen)
		.def_readonly("round_station", &mue::Calculation::Iteration_data::round_station)
		.def_readonly("used_guests", &mue::Calculation::Iteration_data::used_guests)
		.def_readwrite("distance", &mue::Calculation::Iteration_data::distance)
		.def("set_station", &mue::Calculation::Iteration_data::set_station)
		.def("clear_round_data", &mue::Calculation::Iteration_data::clear_round_data);

	class_<mue::Calculation::Guest_candidate>("GuestCandidate", init<mue::Distance, mue::Guest_tuple_generator::GuestPair>())
		.def_readonly("distance", &mue::Calculation::Guest_candidate::distance)
		.def_readonly("guests", &mue::Calculation::Guest_candidate::guests);

	class_<std::vector<mue::Calculation::Guest_candidate> >("GuestCandidateVector")
		.def(vector_indexing_suite<std::vector<mue::Calculation::Guest_candidate> >());

	class_<mue::Calculation>("Calculation", init<unsigned int, mue::Distance_matrix>())
		.def("update_best", &mue::Calculation::update_best)
		.def("initial_round_data", &mue::Calculation::initial_round_data)
		.def("next_round_data", &mue::Calculation::next_round_data)
		.def("dummy_distance", &mue::Calculation::dummy_distance)
		.def("guest_distance", &mue::Calculation::guest_distance)
		.def("host_distance", &mue::Calculation::host_distance)
		.def("determine_guest_candidates", &mue::Calculation::determine_guest_candidates);

	enum_<mue::Calculation::Round>("Round")
		.value("FIRST", mue::Calculation::FIRST)
		.value("SECOND", mue::Calculation::SECOND)
		.value("THIRD", mue::Calculation::THIRD);
}
