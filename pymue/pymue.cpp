#include <vector>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "mue_algorithm.h"
#include "teams.h"
#include "distances.h"
#include "firstround_select.h"

#include "scoped_gil.h"


namespace pymue {
	void calculate_without_gil(mue::Calculation &calculation)
	{
		ScopedGILRelease gilRelease;
		calculation.calculate_distribution();
	}
}


BOOST_PYTHON_MODULE (_pymue)
{
	using namespace boost::python;

	class_<mue::Distance_matrix>("DistanceMatrix", init<unsigned int>())
		.def("set_cost", &mue::Distance_matrix::set_cost)
		.def("min_cost", &mue::Distance_matrix::min_cost)
		.def("lookup", &mue::Distance_matrix::lookup);

	class_<mue::Firstround_team_selection>("FirstroundSelection", init<mue::Distance_matrix, unsigned int>());

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

	def("calculate", pymue::calculate_without_gil);
}
