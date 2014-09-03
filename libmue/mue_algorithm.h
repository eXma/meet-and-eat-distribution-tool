#ifndef MUE_ALGORITHM_H
#define MUE_ALGORITHM_H

#include <random>
#include <cstdint>


#include <boost/assert.hpp>

#include "guest_tuple_iterator.h"
#include "distances.h"
#include "config.h"
#include "seen_table.h"


#define PREDEFINED_RANDOM

namespace mue {



	class Calculation
	{
		public:
			enum Round : std::uint8_t {
				FIRST = 0,
				SECOND = 1,
				THIRD = 2
			};


			struct Round_data
			{
				Calculation::Round   const round;
				std::vector<Team_id> const hosts;
				std::vector<Team_id> const guests;

				std::vector<std::vector<Team_id> > const prev_stations;

				bool first_round() const { return round == Calculation::FIRST; }
				Team_id prev_host(Team_id team) const {
					return prev_stations.back()[team];
				}

				Round_data(Calculation::Round round,
					   std::vector<Team_id> const &hosts,
					   std::vector<Team_id> const &guests,
					   std::vector<std::vector<Team_id> > const &prev_stations)
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
				Guest_tuple_generator::Used_bits used_guests;
				std::vector<Team_id>        round_station;
				Seen_table                  seen_table;

				void set_station(Team_id host, Team_id guest1, Team_id guest2)
				{
					round_station[host] = host;
					round_station[guest1] = host;
					round_station[guest2] = host;

					used_guests.set(guest1);
					used_guests.set(guest2);

					seen_table.add_meeting(host, guest1, guest2);
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
				{}

				bool seen(Team_id host, Team_id guestA, Team_id guestB) const { return seen_table.seen(host, guestA, guestB); }

				void clear_round_data()
				{
					used_guests.reset();
				}
			};


			struct Guest_candidate
			{
				Distance distance;
				Guest_tuple_generator::GuestPair guests;

				Guest_candidate(Distance distance, Guest_tuple_generator::GuestPair const &guests)
				: distance(distance), guests(guests)
				{ }

				bool operator ==(Guest_candidate const &other)
				{
					return other.distance == distance
					    && other.guests.first == guests.first
					    && other.guests.second == guests.second;
				}

				bool operator !=(Guest_candidate const &other)
				{
					return ! (*this == other);
				}

				Guest_candidate() : distance(0), guests(0, 0) { }
			};


		private:

			unsigned int                        const _teamcount;
			unsigned int                        const _teams_per_round;
			std::vector<std::vector<Team_id> >        _round_hosts;
			std::vector<std::vector<Team_id> >        _round_guests;
			Distance_matrix                     const _distance_matrix;
			Distance                                  _best_distance;

#ifndef PREDEFINED_RANDOM
			mutable std::uniform_int_distribution<Team_id> _team_round_random;
			mutable std::mt19937 _random_generator;
#else
			std::vector<Team_id> _team_shuffle;
#endif

			bool _is_round_host(Team_id team, Round round) const
			{
				return     (_teams_per_round * round       <= team)
					&& (_teams_per_round * (round + 1) >  team);
			}

#ifndef PREDEFINED_RANDOM
			Team_id _random_host(Round round) const
			{
				return _team_round_random(_random_generator) + (_teams_per_round * round);
			}
#endif

		public:
			Calculation(unsigned int teamcount, Distance_matrix const &distance_matrix);

			unsigned int teamcount() { return _teamcount; }

			Distance dummy_distance(Team_id host, Guest_tuple_generator::GuestPair const &guests) const;

			Distance guest_distance(Round_data const &round_data, Team_id host, Guest_tuple_generator::GuestPair const &guests) const;

			Distance host_distance(Round_data const &round_data, Team_id host) const;

			std::vector<Guest_candidate> determine_guest_candidates(Round_data const &round_data, Iteration_data const &iteration_data, Team_id current_host) const;

			void update_best(float best) { _best_distance = best; }

			Round_data initial_round_data() const;

			Round_data next_round_data(Round_data const &round_data, Iteration_data const &data) const;

			std::vector<Team_id> round_stations(Round round, Round_data const &round_data, Iteration_data const &iteration_data) const;
	};

	std::ostream& operator<<(std::ostream& os, Calculation::Guest_candidate const& candidate);

}

#endif


