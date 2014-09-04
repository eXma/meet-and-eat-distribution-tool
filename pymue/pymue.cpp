#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "mue_algorithm.h"
#include "teams.h"
#include "distances.h"

#include <vector>


BOOST_PYTHON_MODULE (_pymue)
{
	using namespace boost::python;

	class_<mue::Distance_matrix>("DistanceMatrix", init<unsigned int>())
		.def("set_cost", &mue::Distance_matrix::set_cost)
		.def("lookup", &mue::Distance_matrix::lookup);

	class_<std::vector<mue::Team_id> >("TeamVector")
		.def(vector_indexing_suite<std::vector<mue::Team_id> >());

	class_<mue::Calculation>("Calculation", init<unsigned int, mue::Distance_matrix, mue::Distance>())
		.def("calculate_distribution", &mue::Calculation::calculate_distribution)
		.def("solutions", &mue::Calculation::solutions)
		.def("round_stations", &mue::Calculation::round_stations);

	enum_<mue::Calculation::Round>("Round")
		.value("FIRST", mue::Calculation::FIRST)
		.value("SECOND", mue::Calculation::SECOND)
		.value("THIRD", mue::Calculation::THIRD);
}
