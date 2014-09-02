#ifndef MUE_ALGORITHM_H
#define MUE_ALGORITHM_H

#include <random>


#include <boost/assert.hpp>

#include "guest_tuple_iterator.h"
#include "distances.h"
#include "config.h"


namespace mue {

	class Calculation
	{
		private:
			enum Round {
				FIRST = 0,
				SECOND = 1,
				THIRD = 2
			};


			unsigned int _teamcount;
			unsigned int _teams_per_round;
			Distance_matrix const _distance_matrix;
			std::uniform_int_distribution<Team_id> _team_round_random;
			std::mt19937 _random_generator;


			bool _is_round_host(Team_id team, Round round)
			{
				return     (_teams_per_round * round       <= team)
					&& (_teams_per_round * (round + 1) >  team);
			}

			Team_id _random_host(Round round)
			{
				return _team_round_random(_random_generator) + (_teams_per_round * round);
			}
		
		public:
			Calculation(unsigned int teamcount, Distance_matrix const &distance_matrix);

			Distance dummy_distance(Team_id host, Guest_tuple_generator::GuestPair const &guests);
	};

}

#endif


