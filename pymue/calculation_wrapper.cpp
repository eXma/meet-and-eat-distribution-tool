#include "calculation_wrapper.h"
#include "conversion_helper.h"



static mue::Calculation::Round translate_round(unsigned int round)
{
	switch (round) {
		case 0:
			return mue::Calculation::FIRST;
		case 1:
			return mue::Calculation::SECOND;
		case 2:
			return mue::Calculation::THIRD;
		default:
			break;
	}
	BOOST_ASSERT(false);
	return mue::Calculation::FIRST;
}


pymue::Calculation_wrapper::Calculation_wrapper(unsigned int teamcount, mue::Distance_matrix distances)
:
	mue::Calculation(teamcount, distances)
{ }


pymue::Calculation_wrapper::Round_data_wrapper::Round_data_wrapper(unsigned int round,
					      boost::python::list hosts,
					      boost::python::list guests,
					      boost::python::list prev_stations)
:
	mue::Calculation::Round_data(translate_round(round),
				     translate_vector<mue::Team_id>(hosts),
				     translate_vector<mue::Team_id>(guests),
				     translate_vector<mue::Team_id>(prev_stations))
{
}
