#ifndef MUE_ALGORITHM_H
#define MUE_ALGORITHM_H

#include <random>


#include <boost/assert.hpp>

#include "guest_tuple_iterator.h"
#include "distances.h"
#include "config.h"
#include "seen_table.h"


namespace mue {



	class Calculation
	{
		public:
			enum Round {
				FIRST = 0,
				SECOND = 1,
				THIRD = 2
			};


			struct Round_data
			{
				Calculation::Round   const round;
				std::vector<Team_id> const hosts;
				std::vector<Team_id> const guests;
				std::vector<Team_id> const prev_stations;

				bool first_round() const { return round == Calculation::FIRST; }
				Team_id prev_host(Team_id team) const { return prev_stations[team]; }

				Round_data(Calculation::Round round,
					   std::vector<Team_id> const &hosts,
					   std::vector<Team_id> const &guests,
					   std::vector<Team_id> const &prev_stations)
				:
					round(round),
					hosts(hosts),
					guests(guests),
					prev_stations(prev_stations)
				{ }

				Round_data()
				:
					round(FIRST),
					hosts(),
					guests(),
					prev_stations()
				{}
			};


			struct Iteration_data
			{
				Distance                    distance;
				std::unordered_set<Team_id> used_guests;
				std::vector<Team_id>        round_station;
				Seen_table                  seen_table;

				void set_station(Team_id host, Team_id guest1, Team_id guest2)
				{
					round_station[host] = host;
					round_station[guest1] = host;
					round_station[guest2] = host;
					used_guests.insert(guest1);
					used_guests.insert(guest2);
				}

				Iteration_data(unsigned int teamcount)
				:
					distance(0),
					used_guests(),
					round_station(teamcount),
					seen_table(teamcount)
				{ }

				Iteration_data(unsigned int teamcount, Distance distance)
				:
					distance(distance),
					used_guests(),
					round_station(teamcount),
					seen_table(teamcount)
				{ }

				Iteration_data(Iteration_data const &other)
				:
					distance(other.distance),
					used_guests(other.used_guests),
					round_station(other.round_station),
					seen_table(other.seen_table.clone())
				{ }
			};

			typedef std::pair<Distance, Guest_tuple_generator::GuestPair> Guest_candidate;

		private:

			unsigned int _teamcount;
			unsigned int _teams_per_round;
			Distance_matrix const _distance_matrix;
			mutable std::uniform_int_distribution<Team_id> _team_round_random;
			mutable std::mt19937 _random_generator;
			Distance _best_distance;


			bool _is_round_host(Team_id team, Round round) const
			{
				return     (_teams_per_round * round       <= team)
					&& (_teams_per_round * (round + 1) >  team);
			}

			Team_id _random_host(Round round) const
			{
				return _team_round_random(_random_generator) + (_teams_per_round * round);
			}
		
		public:
			Calculation(unsigned int teamcount, Distance_matrix const &distance_matrix);

			unsigned int teamcount() { return _teamcount; }

			Distance dummy_distance(Team_id host, Guest_tuple_generator::GuestPair const &guests) const;

			Distance guest_distance(Round_data const &round_data, Team_id host, Guest_tuple_generator::GuestPair const &guests) const;

			std::vector<Guest_candidate> determine_guest_candidates(Round_data const &round_data, Iteration_data const &iteration_data, Team_id current_host) const;

	};


}

#endif


