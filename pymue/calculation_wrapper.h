#ifndef PYMUE_ROUND_DATA_WRAPPER
#define PYMUE_ROUND_DATA_WRAPPER


#include <boost/python/list.hpp>


#include "mue_algorithm.h"


namespace pymue {


	class Calculation_wrapper : public mue::Calculation
	{

		public:

			struct Round_data_wrapper : mue::Calculation::Round_data
			{
				Round_data_wrapper(unsigned int round,
						   boost::python::list hosts,
						   boost::python::list guests,
						   boost::python::list prev_stations);

			};


			Calculation_wrapper(unsigned int teamcount, mue::Distance_matrix distances);

			mue::Distance guest_distance(Round_data_wrapper const &round_data, mue::Team_id host, mue::Guest_tuple_generator::GuestPair const &guests)
			{
				BOOST_ASSERT(!round_data.first_round() ||  round_data.prev_stations.size() == teamcount());
				return mue::Calculation::guest_distance(round_data, host, guests);
			}

	};
}

#endif
